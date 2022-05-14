#pragma once

class ProcessProtecter
{
public:
	static bool protect(int pid);

	static bool unprotect();

	static bool isProtected();

private:

	static bool _protected;
};

