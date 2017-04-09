// processestable.h

#ifndef PROCESSESTABLE_H
#define PROCESSESTABLE_H

#include "syscall.h"
#include "thread.h"
#include <map>
#include <stdexcept>

class ProcessesTable {
  public:
    ProcessesTable();
    ~ProcessesTable();

    SpaceId Insert(Thread *thread);
    void Delete(SpaceId spaceId);
    Thread *GetProcess(SpaceId spaceId);
    SpaceId GetSpaceId(Thread *thread);
    bool Zombie(SpaceId spaceId);
    void SetExitValue(SpaceId spaceId, int exitValue);
    int GetExitValue(SpaceId spaceId);

  private:
    std::map<SpaceId, Thread *> table;
    // Los valores de exit deben guardarse por separado de los threads ya que deben conservarse
    // aunque el proceso ya haya sido eliminado de table. De lo contrario la entrada de exitValues
    // sería eliminada antes de que HandlerJoin pudiera obtener el valor de exit.
    // Buscar una mejor solución para evitar que crezca indefinidamente!!!
    std::map<SpaceId, int> exitValues;
    SpaceId nextId;
};

#endif // PROCESSESTABLE_H
