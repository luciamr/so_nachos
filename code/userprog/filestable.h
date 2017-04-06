// filestable.h

#ifndef FILESTABLE_H
#define FILESTABLE_H

#include "openfile.h"
#include "syscall.h"
#include <map>
#include <stdexcept>

class FilesTable {
  public:
    FilesTable();
    ~FilesTable();// filestable.h

#ifndef FILESTABLE_H
#define FILESTABLE_H

#include "openfile.h"
#include "syscall.h"
#include <map>
#include <stdexcept>

class FilesTable {
  public:
    FilesTable();
    ~FilesTable();

    OpenFileId Insert(OpenFile *file);
    void Delete(OpenFileId fileId);
    OpenFile *Get(OpenFileId fileId);

  private:
    std::map<OpenFileId, OpenFile *> table;
    OpenFileId nextId;
};

#endif // FILESTABLE_H


    OpenFileId Insert(OpenFile *file);
    void Delete(OpenFileId fileId);
    OpenFile *Get(OpenFileId fileId);

  private:
    std::map<OpenFileId, OpenFile *> table;
    OpenFileId nextId;
};

#endif // FILESTABLE_H
