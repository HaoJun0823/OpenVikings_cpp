#pragma once
namespace NC2Logic
{
    void ToUserMessage_Construct(void* message);
    void ToUserMessage_FinishMessage(void* message);
}
namespace NC2LogicExternals
{
    void External_Misc_SendToUserMessage(const void* message);
}
