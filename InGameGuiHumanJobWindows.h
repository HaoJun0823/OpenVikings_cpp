#pragma once

#include "StringInputHandler.h"
#include <cstddef>
#include <cstdint>

namespace NC2InGameGuiSelectedAction
{
	// Documentary Windows state tails; constructors still operate on the original storage.
	struct SSelectedHumanActionWindowStateStorage
	{
		std::uint8_t validSelection;
		std::byte unclassified01[3];
		std::uint32_t selectionChangeCount;
	};
	struct SChangeJobWindowStateStorage
	{
		SSelectedHumanActionWindowStateStorage selection;
		std::uint32_t lastRefreshTime;
		void* jobList;
	};
	struct SChangeNameWindowStateStorage
	{
		SSelectedHumanActionWindowStateStorage selection;
		NC2InGameGuiBaseTextInputElement::CStringInputHandler inputHandler;
	};

	void* SelectedSingleHumanChangeJobWindow_Construct(void* window);
	void* SelectedSingleHumanChangeNameWindow_Construct(void* window);
	void* SelectedSingleHumanLearnJobWindow_Construct(void* window);
	void* SelectedSingleHumanChangeProducedGoodWindow_Construct(void* window);
}
