#include "ScriptComboBox.h"
#include "ScriptManager.h"

ScriptComboBox::ScriptComboBox(QWidget* parent)
	:QComboBox(parent)
{
	for (auto i : ScriptManager::instance()->scripts())
		addItem(i.name);
}