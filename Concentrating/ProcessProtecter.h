#pragma once

class ProcessProtecter
{
public:
	static bool protect(int pid);

	static bool unprotect();
};

