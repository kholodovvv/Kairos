#include <QApplication>
#include "include/core.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
	Core core;

	return core.ObjectsInit() == true ? app.exec() : 1;
}