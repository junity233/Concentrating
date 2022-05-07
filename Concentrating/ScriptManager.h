#pragma once

#include <qstring.h>
#include <qmap.h>

class ScriptManager
{
	ScriptManager() {}

public:

	struct Script {
		QString name;
		QString code;
	};

	using ScriptList = QList<Script>;

	static ScriptManager* instance(){ return _instance; }

	ScriptList & scripts() { return _scripts; }
	
	Script& script(int index) { return _scripts[index]; }

	int scriptCount()const { return _scripts.size(); }

	void append(const QString& name, const QString& script);
	void remove(int index);

	bool load(const QString& file);
	bool save(const QString& file);

private:
	ScriptList _scripts;

	static ScriptManager* _instance;
};

