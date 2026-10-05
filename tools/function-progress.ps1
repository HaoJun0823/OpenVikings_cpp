# Read-only Windows source/ASM/PE inventory; output artifacts are separate from game builds.
param([string]$WorkspacePath = (Split-Path -Parent $PSScriptRoot), [string]$AssemblyDirectory = (Join-Path (Split-Path -Parent (Split-Path -Parent $PSScriptRoot)) 'OpenVikings_ASM'), [string]$ArtifactPath = (Join-Path $env:TEMP ('OpenVikings-function-progress-' + (Get-Date -Format 'yyyyMMdd'))))
$ErrorActionPreference='Stop'
$asmPath=Join-Path $AssemblyDirectory 'assembly_weltwunder_win.asm'
New-Item -ItemType Directory -Force -Path $artifactPath | Out-Null
function Mask-Code([string]$source) {
    [regex]::Replace($source,'(?s)/\*.*?\*/|(?m)//[^\r\n]*|(?s)"(?:\\.|[^"\\])*"|''(?:\\.|[^''\\])*''', [System.Text.RegularExpressions.MatchEvaluator]{param($match) [regex]::Replace($match.Value,'[^\r\n]',' ')})
}
function Get-Key([string]$signature) {
    $match=[regex]::Match($signature,'(?<name>(?:[A-Za-z_]\w*::)*(?:operator=(?=\s*\()|~?[A-Za-z_]\w*))\s*\((?<params>[^()]*)\)')
    if(-not $match.Success){return $null}
    $name=($match.Groups['name'].Value -split '::')[-1]
    if($name -in @('if','for','while','switch','catch','static_assert','sizeof','alignof','decltype','__declspec')){return $null}
    $params=@()
    foreach($parameter in ($match.Groups['params'].Value -split ',')) {
        $type=($parameter -split '=')[0].Trim()
        if($type -match '\s+\w+\s*(?:\[[^\]]*\])?$' -and $type -notmatch '^(?:const\s+)?(?:unsigned|signed|long|short)(?:\s+(?:int|long))?$') { $type=[regex]::Replace($type,'\b\w+\s*(?:\[[^\]]*\])?$','') }
        $type=[regex]::Replace($type,'(?:[A-Za-z_]\w*::)+','')
        $type=[regex]::Replace($type,'\s+','').Replace('const','')
        $params+=$type
    }
    return "$name($($params -join ','))"
}
$entries=[Collections.Generic.List[object]]::new()
$definitionPattern='(?m)^[ \t]*(?<sig>[^\r\n;{}]*\([^;{}]*\)[^;{}]*?)(?<end>;|\{)'
foreach($file in Get-ChildItem -LiteralPath $workspacePath -File | Where-Object {$_.Extension -in '.cpp','.h' -and $_.Name -notin @('dplay8.h','dpaddr.h','TemporaryLinkStubs.cpp')}) {
    $source=[IO.File]::ReadAllText($file.FullName)
    $masked=Mask-Code $source
    foreach($match in [regex]::Matches($masked,$definitionPattern)) {
        $signature=$source.Substring($match.Groups['sig'].Index,$match.Groups['sig'].Length).Trim()
        $key=Get-Key $match.Groups['sig'].Value
        if(-not $key -or $signature -match '^(?:if|for|while|switch|catch|return|else|case|do|alignas|static_assert|using|typedef|assert|__asm)\b'){continue}
        $prefixLines=($source.Substring(0,$match.Index) -split '\r?\n')
        $comment=@()
        for($line=$prefixLines.Count-1;$line -ge 0;$line--) {
            if($prefixLines[$line] -match '^\s*(?://.*)?$'){ $comment=@($prefixLines[$line])+$comment } else {break}
        }
        $addresses=[Collections.Generic.HashSet[uint32]]::new()
        foreach($commentLine in $comment) {
            if($commentLine -match '^\s*//\s*(?:Original (?:function|functions|virtual entry|deleting destructor|constructor|constructors|getter|predicate|entry)|ABI adapter)\b') {
                foreach($address in [regex]::Matches(($commentLine -split ';')[0],'(?:FUN_|LAB_)?(004[0-9a-fA-F]{5})\b')) { [void]$addresses.Add([Convert]::ToUInt32($address.Groups[1].Value,16)) }
            }
        }
        if($key -match '^FUN_([0-9a-fA-F]{8})\('){ [void]$addresses.Add([Convert]::ToUInt32($Matches[1],16)) }
        $isDefinition=$match.Groups['end'].Value.Trim() -eq '{'
        $rawCalls=@(); $body=''
        if($isDefinition) {
            $start=$match.Groups['end'].Index+$match.Groups['end'].Length-1
            $depth=1;$pos=$start+1
            while($pos -lt $masked.Length -and $depth -gt 0){if($masked[$pos] -eq '{'){$depth++}elseif($masked[$pos] -eq '}'){$depth--};$pos++}
                        if($signature -match '\)\s*:') {
                while($start -gt 0 -and $masked.Substring($match.Index,$start-$match.Index).TrimEnd() -match '\b\w+$') {
                    $following=$pos
                    while($following -lt $masked.Length -and [char]::IsWhiteSpace($masked[$following])){$following++}
                    if($masked[$following] -ne ',' -and $masked[$following] -ne '{'){break}
                    $start=$masked.IndexOf('{',$following)
                    $depth=1;$pos=$start+1
                    while($pos -lt $masked.Length -and $depth -gt 0){if($masked[$pos] -eq '{'){$depth++}elseif($masked[$pos] -eq '}'){$depth--};$pos++}
                }
            }
            $body=$masked.Substring($start,$pos-$start)
            $rawCalls=@([regex]::Matches($body,'\bFUN_([0-9a-fA-F]{8})\s*\(') | ForEach-Object {[Convert]::ToUInt32($_.Groups[1].Value,16)} | Sort-Object -Unique)
        }
        $entries.Add([pscustomobject]@{File=$file.Name;Base=$file.BaseName;Key=$key;Signature=$signature;Line=1+[regex]::Matches($source.Substring(0,$match.Index),'\n').Count;Definition=$isDefinition;Addresses=@($addresses);RawCalls=$rawCalls;Body=$body})
    }
}
$definitions=@($entries | Where-Object Definition)
$unmatchedDeclarations=[Collections.Generic.List[object]]::new()
foreach($declaration in $entries | Where-Object {-not $_.Definition -and $_.Addresses.Count -gt 0}) {
    $matches=@($definitions | Where-Object {$_.Key -eq $declaration.Key -and ($_.Base -eq $declaration.Base -or $_.Signature -match '::')})
    $sameFile=@($matches | Where-Object {$_.Base -eq $declaration.Base})
    if($sameFile.Count -gt 0){$matches=$sameFile}
    if($matches.Count -eq 1){$matches[0].Addresses=@($matches[0].Addresses+$declaration.Addresses | Sort-Object -Unique)}
    else { $unmatchedDeclarations.Add([pscustomobject]@{File=$declaration.File;Line=$declaration.Line;Key=$declaration.Key;Addresses=($declaration.Addresses | ForEach-Object {'{0:X8}' -f $_}) -join ',';Candidates=$matches.Count}) }
}
$unfinished=@(Import-Csv (Join-Path $workspacePath 'docs/TEMPORARY_LINK_STUBS.csv'))
$excludedAddresses=[Collections.Generic.HashSet[uint32]]::new()
foreach($entry in $unfinished){[void]$excludedAddresses.Add([Convert]::ToUInt32($entry.Address,16))}
$implemented=[Collections.Generic.HashSet[uint32]]::new()
$rows=foreach($definition in $definitions | Where-Object {$_.Addresses.Count -gt 0}) {
    foreach($address in $definition.Addresses) { if($address -lt 0x401000 -or $address -ge 0x4f3000){continue}; if($excludedAddresses.Contains($address)){continue}; [void]$implemented.Add($address); [pscustomobject]@{Address='{0:X8}' -f $address;File=$definition.File;Line=$definition.Line;Signature=$definition.Signature;RawCallees=($definition.RawCalls | ForEach-Object {'{0:X8}' -f $_}) -join ','} }
}
$rows | Sort-Object Address,File,Line | Export-Csv (Join-Path $artifactPath 'implemented-addresses.csv') -NoTypeInformation -Encoding utf8
$unmatchedDeclarations | Export-Csv (Join-Path $artifactPath 'unmatched-declarations.csv') -NoTypeInformation -Encoding utf8
$definitions | Where-Object {$_.Addresses.Count -eq 0} | Select-Object File,Line,Key,Signature | Export-Csv (Join-Path $artifactPath 'definitions-without-address.csv') -NoTypeInformation -Encoding utf8
$asm=[IO.File]::ReadAllText($asmPath)
$directCalls=[Collections.Generic.HashSet[uint32]]::new()
foreach($call in [regex]::Matches($asm,'(?im)^([0-9a-f]{8})\s*\|\s*E8\s+([0-9a-f]{8})\s*\|\s*call\b')) {
    $address=[Convert]::ToUInt32($call.Groups[1].Value,16)
    $hex=$call.Groups[2].Value
    $bytes=0..3 | ForEach-Object {[Convert]::ToByte($hex.Substring($_*2,2),16)}
    $target=[long]$address+5+[BitConverter]::ToInt32([byte[]]$bytes,0)
    if($target -ge 0x401000 -and $target -lt 0x4f3000){[void]$directCalls.Add([uint32]$target)}
}
$pointerCode=[Collections.Generic.HashSet[uint32]]::new()
foreach($ref in [regex]::Matches($asm,'(?im)^[0-9a-f]{8}\s*\|[^\r\n]*?\|\s*(?:push|mov|lea)\s+[^|\r\n]*?<game\.sub_([0-9a-f]{6,8})>')){[void]$pointerCode.Add([Convert]::ToUInt32($ref.Groups[1].Value,16))}
$peBytes=[IO.File]::ReadAllBytes((Join-Path $AssemblyDirectory 'Game.exe'))
$pe=[BitConverter]::ToInt32($peBytes,0x3c);$sectionCount=[BitConverter]::ToUInt16($peBytes,$pe+6);$sectionBase=$pe+24+[BitConverter]::ToUInt16($peBytes,$pe+20)
$sections=for($index=0;$index -lt $sectionCount;$index++){$s=$sectionBase+40*$index;[pscustomobject]@{VA=[uint32](0x400000+[BitConverter]::ToUInt32($peBytes,$s+12));Size=[BitConverter]::ToUInt32($peBytes,$s+16);Raw=[BitConverter]::ToUInt32($peBytes,$s+20);Name=[Text.Encoding]::ASCII.GetString($peBytes,$s,8).Trim([char]0)}}
$tableBases=[Collections.Generic.HashSet[uint32]]::new()
foreach($ref in [regex]::Matches($asm,'(?im)^[0-9a-f]{8}\s*\|[^\r\n]*?\|\s*mov\s+dword ptr[^|\r\n]*?<game\.&sub_')) {
    $byteField=($ref.Value -split '\|')[1] -replace '\s',''
    if($byteField.Length -ge 8){$hex=$byteField.Substring($byteField.Length-8);$b=0..3 | ForEach-Object {[Convert]::ToByte($hex.Substring($_*2,2),16)};$base=[BitConverter]::ToUInt32([byte[]]$b,0);if($base -ge 0x4f3000 -and $base -lt 0x510000){[void]$tableBases.Add($base)}}
}
$virtualEntries=[Collections.Generic.HashSet[uint32]]::new()
foreach($base in $tableBases) {
    $section=$sections | Where-Object {$base -ge $_.VA -and $base -lt $_.VA+$_.Size} | Select-Object -First 1
    if(-not $section){continue}
    $offset=$section.Raw+$base-$section.VA
    for($index=0;$index -lt 128;$index++){
        $target=[BitConverter]::ToUInt32($peBytes,$offset+4*$index)
        if($target -lt 0x401000 -or $target -ge 0x4f3000){break}
        [void]$virtualEntries.Add($target)
    }
}
$known=[Collections.Generic.HashSet[uint32]]::new($directCalls)
foreach($set in @($pointerCode,$virtualEntries,$implemented,$excludedAddresses)){foreach($address in $set){[void]$known.Add($address)}}
$rawReferenced=[Collections.Generic.HashSet[uint32]]::new()
foreach($definition in $definitions){foreach($address in $definition.RawCalls){[void]$rawReferenced.Add($address)}}
$pendingRaw=@($rawReferenced | Where-Object {-not $implemented.Contains($_)})
$rawResolved=@($rawReferenced | Where-Object {$implemented.Contains($_)})
$importThunks=[Collections.Generic.HashSet[uint32]]::new()
$iatRva=[BitConverter]::ToUInt32($peBytes,$pe+24+96+12*8)
$iatSize=[BitConverter]::ToUInt32($peBytes,$pe+24+96+12*8+4)
foreach($address in @($known)) {
    $section=$sections | Where-Object {$address -ge $_.VA -and $address+6 -le $_.VA+$_.Size} | Select-Object -First 1
    if(-not $section){continue}
    $offset=$section.Raw+$address-$section.VA
    if($peBytes[$offset] -eq 0xff -and $peBytes[$offset+1] -eq 0x25){
        $pointer=[BitConverter]::ToUInt32($peBytes,$offset+2)
        if($pointer -ge 0x400000+$iatRva -and $pointer -lt 0x400000+$iatRva+$iatSize){[void]$importThunks.Add($address);[void]$known.Remove($address)}
    }
}
$knownRows=foreach($address in $known){[pscustomobject]@{Address='{0:X8}' -f $address;DirectCall=$directCalls.Contains($address);CodePointer=$pointerCode.Contains($address);VirtualTable=$virtualEntries.Contains($address);MappedImplementation=$implemented.Contains($address);RawCalledInCpp=$rawReferenced.Contains($address)}}
$knownRows | Sort-Object Address | Export-Csv (Join-Path $artifactPath 'windows-entry-inventory.csv') -NoTypeInformation -Encoding utf8
$summary=[pscustomobject]@{TrackedUnfinished=$unfinished.Count;ParsedCppDefinitions=$definitions.Count;MappedAddresses=$implemented.Count;DirectCalls=$directCalls.Count;ExplicitCodePointers=$pointerCode.Count;VirtualTables=$tableBases.Count;VirtualEntries=$virtualEntries.Count;KnownEntries=$known.Count;ExcludedApiThunks=$importThunks.Count;MappedDirectCalls=@($directCalls | Where-Object {$implemented.Contains($_)}).Count;OpenKnownEntries=$known.Count-$implemented.Count;RawReferenced=$rawReferenced.Count;RawUnmapped=$pendingRaw.Count;RawAlreadyMapped=$rawResolved.Count;DeclarationsWithoutUniqueBody=$unmatchedDeclarations.Count;Artifacts=$artifactPath}
$summary | ConvertTo-Json | Set-Content (Join-Path $artifactPath 'summary.json')
$summary | Format-List
