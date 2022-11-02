#include "server.hpp"

using namespace rnitta;

int main()
{
	try
	{
		Server server;

		server.mainLoop();
	} catch (const std::runtime_error &e)
	{
		std::cerr << e.what() << std::endl;
		exit(1);
	}
}
