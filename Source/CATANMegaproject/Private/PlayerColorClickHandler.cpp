#include "PlayerColorClickHandler.h"
#include "DebugUserWidget.h"

void UPlayerColorClickHandler::HandleClick()
{
    if (ParentWidget)
    {
        ParentWidget->OnPlayerCardClicked(PlayerColor);
    }
}
