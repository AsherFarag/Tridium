#pragma once

class Delegate
{
public:
	void Bind() {}
	/* Calls the binded function */
	void Call() {}

private:
};

class SharedDelegate
{
public:
	void Bind() {}
	/* Calls the binded functions */
	void Broadcast() {}

private:
};