// processestable.cc

#include "processestable.h"

//----------------------------------------------------------------------
// ProcessesTable::ProcessesTable
//	Inicializa la tabla de procesos en ejecución.
//----------------------------------------------------------------------

ProcessesTable::ProcessesTable()
{
	table = std::map<SpaceId, Thread *>();
	nextId = 0;
}

ProcessesTable::~ProcessesTable()
{
	table.clear();
}

//----------------------------------------------------------------------
// ProcessesTable::Insert
//  Toma un Thread y le asigna un SpaceId, registrándolo en la tabla.
//----------------------------------------------------------------------

SpaceId
ProcessesTable::Insert(Thread *thread)
{
	int currentId = nextId;

	table[currentId] = thread;
	DEBUG('a', "Process inserted: SpaceId -> %d.\n", currentId);
	nextId++; //seteo Id para el próximo Insert

    return currentId;
}

//----------------------------------------------------------------------
// ProcessesTable::Delete
// 	Elimina de la tabla la entrada correspondiente a un SpaceId.
//----------------------------------------------------------------------

void
ProcessesTable::Delete(SpaceId spaceId)
{
    table.erase(spaceId);
	DEBUG('a', "Process deleted: SpaceId -> %d.\n", spaceId);
}

//----------------------------------------------------------------------
// ProcessesTable::GetProcess
// 	Devuelve el Thread correspondiente a un SpaceId.
//  Retorna NULL si no lo encuentra.
//----------------------------------------------------------------------

Thread *
ProcessesTable::GetProcess(SpaceId spaceId)
{
	Thread *thread;

	try {
		thread = table.at(spaceId);
		DEBUG('a', "Process found: SpaceId -> %d.\n", spaceId);
	}
	catch(const std::out_of_range& e) {
		thread = NULL;
		DEBUG('a', "Process NOT found: SpaceId -> %d.\n", spaceId);
	}

	return thread;
}

//----------------------------------------------------------------------
// ProcessesTable::GetSpaceId
// 	Devuelve el SpaceId correspondiente a un Thread.
//  Retorna -1 si no lo encuentra.
//----------------------------------------------------------------------

SpaceId
ProcessesTable::GetSpaceId(Thread *thread)
{
	SpaceId spaceId;
	std::map<SpaceId, Thread *>::iterator i;

	// confirmar si es seguro el == sobre los punteros
	for (i = table.begin(); i != table.end() && i->second != thread; i++);

	if (i->second == thread) {
		spaceId = i->first;
		DEBUG('a', "Process found: SpaceId -> %d.\n", spaceId);
	}
	else {
		spaceId = -1;
		DEBUG('a', "Process NOT found: SpaceId -> %d.\n", spaceId);
	}

	return spaceId;
}



//----------------------------------------------------------------------
// ProcessesTable::Zombie
// 	Determina si el proceso correspondiente al SpaceId recibido es un Zombie.
//----------------------------------------------------------------------

bool
ProcessesTable::Zombie(SpaceId spaceId)
{
	Thread *thread;
	bool zombie;

	try {
		thread = table.at(spaceId);
		zombie = thread == NULL;
		if (zombie)
			DEBUG('a', "Process is a Zombie: SpaceId -> %d.\n", spaceId);
		else
			DEBUG('a', "Process is NOT a Zombie: SpaceId -> %d.\n", spaceId);
	}
	catch(const std::out_of_range& e) {
		zombie = true;
		DEBUG('a', "Process NOT found: SpaceId -> %d.\n", spaceId);
	}

	return zombie;
}

//----------------------------------------------------------------------
// ProcessesTable::SetExitValue
//  Toma un SpaceId y un int, registrando en la tabla el valor de exit
//  correspondiente a ese proceso.
//----------------------------------------------------------------------

void
ProcessesTable::SetExitValue(SpaceId spaceId, int exitValue)
{
	exitValues[spaceId] = exitValue;
	DEBUG('a', "Exit value inserted: SpaceId -> %d, ExitValue -> %d.\n", spaceId, exitValue);
}

//----------------------------------------------------------------------
// ProcessesTable::GetExitValue
// 	Devuelve el valor de exit correspondiente a un SpaceId.
//  Retorna -1 si no lo encuentra. Ver!!!
//----------------------------------------------------------------------

int
ProcessesTable::GetExitValue(SpaceId spaceId)
{
	int value;

	try {
		value = exitValues.at(spaceId);
		DEBUG('a', "Process found: SpaceId -> %d.\n", spaceId);
	}
	catch(const std::out_of_range& e) {
		value = -1;
		DEBUG('a', "Process NOT found: SpaceId -> %d.\n", spaceId);
	}

	return value;
}
