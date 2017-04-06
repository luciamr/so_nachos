// filestable.cc

#include "filestable.h"

//----------------------------------------------------------------------
// FilesTable::FilesTable
//	Inicializa la tabla de archivos abiertos de un proceso.
//----------------------------------------------------------------------

FilesTable::FilesTable()
{
	table = std::map<OpenFileId, OpenFile *>();
	nextId = 2; //0: Input, 1: Output
}

FilesTable::~FilesTable()
{
	table.clear();
}

//----------------------------------------------------------------------
// FilesTable::Insert
//  Toma un OpenFile y le asigna un OpenFileId, registrándolo en la tabla.
//----------------------------------------------------------------------

OpenFileId
FilesTable::Insert(OpenFile *file)
{
	int currentId = nextId;

	table[currentId] = file;
	DEBUG('a', "File inserted: OpenFileId -> %d.\n", currentId);
	nextId++; //seteo Id para el próximo Insert

    return currentId;
}

//----------------------------------------------------------------------
// FilesTable::Delete
// 	Elimina de la tabla la entrada correspondiente a un OpenFileId.
//----------------------------------------------------------------------

void
FilesTable::Delete(OpenFileId fileId)
{
    table.erase(fileId);
	DEBUG('a', "File deleted: OpenFileId -> %d.\n", fileId);
}

//----------------------------------------------------------------------
// FilesTable::Get
// 	Devuelve el OpenFile correspondiente a un OpenFileId.
//----------------------------------------------------------------------

OpenFile *
FilesTable::Get(OpenFileId fileId)
{
	OpenFile *file;

	try {
		file = table.at(fileId);
		DEBUG('a', "File found: OpenFileId -> %d.\n", fileId);
	}
	catch(const std::out_of_range& e) {
		file = NULL;
		DEBUG('a', "File not found: OpenFileId -> %d.\n", fileId);
	}

	return file;
}
