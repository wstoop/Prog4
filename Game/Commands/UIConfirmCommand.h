#include "Commands/Command.h"
#include "UISelection.h"


class UIConfirmCommand : public Command
{
public:
    UIConfirmCommand(UISelection* selection) : m_pSelection(selection) {}

    void Execute() override
    {
        auto* comp = m_pSelection->current->GetComponent<dae::SelectableButtonComponent>();
        if (comp) comp->Activate();
    }
private:
    UISelection* m_pSelection;
};