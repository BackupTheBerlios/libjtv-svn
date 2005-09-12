/*
    Shared Class Facility (SCF)
    Copyright (C) 1999 by Andrew Zabolotny

    Base SCF definitions, macros and interfaces.
*/

#ifndef __SCF_H__
#define __SCF_H__

#include "scf/scfdefs.h"

/**
 * ������ ��� ���������� ������: �������� #define SCF_DEBUG � ������
 * ������������� ��� ������ � ����������������; ��� ���� ��� ��������
 * �������� SCF ����� �������� ���������� ����������.
 */
#ifdef SCF_DEBUG
#  define SCF_TRACE(x)							\
   {									\
     printf ("SCF [%s:%d]:\n", __FILE__, __LINE__);			\
     printf x; SCF_PRINT_CALL_ADDRESS					\
   }
#else
#  define SCF_TRACE(x)
#endif

/**
 * ������ ��� ������ ������, ������ ������� ���������.
 * �������� ������ � GCC >= 2.8.0.
 */
#if (__GNUC__ >= 2) && (__GNUC_MINOR__ >= 8)
#  define SCF_PRINT_CALL_ADDRESS					\
   printf ("  Called from address %p\n", __builtin_return_address (0));
#else
#  define SCF_PRINT_CALL_ADDRESS
#endif

/// ������ ��� ���������� ������ ������ ����������.
#define SCF_CONSTRUCT_VERSION(Major,Minor,Micro)			\
  ((Major << 24) | (Minor << 16) | Micro)

/**
 * SCF_VERSION ����� ���� ����������� ��� ����� ������� ����� �������
 * ������ ������ ����������; �� ������ ������� ��� ���������� � ���
 * ��������� ����� (major/minor/micro) ������ ������ ����������.
 * ��������:
 * <pre>
 * SCF_VERSION (iSomething, 0, 0, 1);
 * struct iSomething : public iBase
 * {
 *   ...
 * };
 * </pre>
 * ����������� ���� ������ ���������� ��������� iSomething ������ 0.0.1.
 */
#define SCF_VERSION(Name,Major,Minor,Micro)				\
  const int VERSION_##Name = SCF_CONSTRUCT_VERSION (Major, Minor, Micro)

SCF_VERSION (iBase, 0, 0, 1);

/**
 * iBase �������� ������� �����������; ��� ��������� ���������� ������
 * ���� ���������� �� ���� ��� ��� �����������. ��� ��������� ��������
 * � ��������� ���������� �������� SCF ���������� �� �� �������
 * ��������� ����������.
 */
struct iBase
{
  /**
   * ��������� ������� ������ �� ������ ������.
   * ������ ���, ����� �� �������� ��������� �� ����� ������ ���
   * ����������� ��������, ���������� �������� ��� ������� ��� ����,
   * ����� ������ ���� � ���, ��� �� ��� ��� ����-�� �����. ��� ����
   * �� �� �������� ������ ���������� �� ����, ��� ������� ������
   * ������ ������ ����.
   */
  virtual void IncRef () = 0;
  /**
   * ��������� ������� ������ �� ������ ������.
   * ����� �� ��������� ������ � ��������� ��������, ������ �����������
   * ���������� ������� ��� �������. ��� ���������� ��������� ������ ����
   * ������ ������������� ��������������. ������������� ����� ���������
   * ����������� ����������� ���������� ������� �� ����, ��� �� ���������
   * ���� ������.
   */
  virtual void DecRef () = 0;
  /**
   * ��������� ���� �� ����������� �������������� ���� ��������.
   * ��� ���� � �������� ��������� ���������� ������������� ����������
   * (� ���� ������) � ��� ������. ���� ������ �������� �����������
   * ����������� ����������� ������ (�������� ������������� ���������
   * �������� scfCompatibleVersion), QueryInterface ���������� ���������
   * �� ���� ���������, ����� ������� ���������� NULL. ��� ��������
   * ������������� ���� ������� �� �������������, ������ ������������
   * ������ QUERY_INTERFACE.
   */
  virtual void *QueryInterface (const char *iInterfaceID, int iVersion) = 0;
};

/**
 * ���� ������ ����� ������������ ������ ����������� ������,
 * ��������������� ��������� SCF ��� ����������� ������� �������
 * �������������� �� ������ iBase.
 */
#define DECLARE_IBASE							\
  DECLARE_EMBEDDED_IBASE (iBase)

/**
 * DECLARE_EMBEDDED_IBASE ������������ ��� ����������� ������� �������
 * �������������� �� ������ iBase ������ ���������� ������ ������� �����
 * ����� SCF ���������. OuterClass ��� ��� ������������� ������.
 */
#define DECLARE_EMBEDDED_IBASE(OuterClass)				\
public:									\
  int scfRefCount;		/* Reference counter */			\
  OuterClass *scfParent;	/* The parent object */			\
  virtual void IncRef ();						\
  virtual void DecRef ();						\
  virtual void *QueryInterface (const char *iInterfaceID, int iVersion)

/**
 * ������ CONSTRUCT_IBASE ������ ���������� �� ������������ SCF �������
 * (�� �� �� ������������� ��������� �������). ������ ������ �����
 * ����������� ����� �������� ���� iBase* (������������ � �����������
 * �������� scfCreateInstance), ������� � ���������� � �������� ���������
 * ����� �������. ������ �������� �������� ������� ������ � ��������������
 * ��������� �� ������-��������.
 */
#define CONSTRUCT_IBASE(Parent)						\
  scfRefCount = 1; if ((scfParent = Parent)) scfParent->IncRef();

/**
 * ������ CONSTRUCT_EMBEDDED_IBASE ���������� �� ������������ ���������������
 * ������, �������� ��������� SCF ����������. ������ �������� ������� ������
 * ���������� ������� � ������������� ��� ��������� �� ������������ ������
 * �� ����.
 */
#define CONSTRUCT_EMBEDDED_IBASE(Interface)				\
  Interface.scfRefCount = 0; Interface.scfParent = this;

/**
 * ���� ������ ��������� ����������� ������� ���������������� (��������������
 * �� iBase). ������ ����, ����� ���������� ��� ��� IncRef(), DecRef() �
 * ��� ����� ����� ������ ������� IMPLEMENT_IBASE � ����� ���������� -
 * ����� ������.
 */
#define IMPLEMENT_IBASE(Class)						\
void Class::IncRef ()							\
{									\
  SCF_TRACE (("  (%s *)%p->IncRef (%d)\n", #Class, this, scfRefCount + 1));\
  if (scfParent)							\
    scfParent->IncRef ();						\
  scfRefCount++;							\
}									\
void Class::DecRef ()							\
{									\
  scfRefCount--;							\
  if (scfParent)							\
    scfParent->DecRef ();						\
  if (scfRefCount <= 0)							\
  {									\
    SCF_TRACE (("  delete (%s *)%p\n", #Class, this));			\
    delete this;							\
  }									\
  else									\
    SCF_TRACE (("  (%s *)%p->DecRef (%d)\n", #Class, this, scfRefCount));\
}									\
void *Class::QueryInterface (const char *iInterfaceID, int iVersion)	\
{									\
  SCF_TRACE (("  (%s *)%p->QueryInterface (%s, %08X)\n",		\
    #Class, this, iInterfaceID, iVersion));				\

/**
 * IMPLEMENT_EMBEDDED_IBASE ������ �� ��, ��� � IMPLEMENT_IBASE,
 * �� ��� ��������� �����������. �� ���������� �� ���� ���, ���
 * ��� ���������� ��� �������� ������ �� ���� ��� �� �����
 * ����������� ���� (��� ��� ��� ����� ������� ��� ��������
 * ������������� �������).
 */
#define IMPLEMENT_EMBEDDED_IBASE(Class)					\
void Class::IncRef ()							\
{									\
  SCF_TRACE (("  (%s *)%p->IncRef (%d)\n", #Class, this, scfRefCount + 1));\
  if (scfParent)							\
    scfParent->IncRef ();						\
  scfRefCount++;							\
}									\
void Class::DecRef ()							\
{									\
  scfRefCount--;							\
  if (scfParent)							\
    scfParent->DecRef ();						\
  SCF_TRACE (("  (%s *)%p->DecRef (%d)\n", #Class, this, scfRefCount));	\
}									\
void *Class::QueryInterface (const char *iInterfaceID, int iVersion)	\
{									\
  SCF_TRACE (("  (%s *)%p->QueryInterface (%s, %08X)\n",		\
    #Class, this, iInterfaceID, iVersion));				\

/**
 * ���� ������ ������������ ��� ��������� ���������� IMPLEMENT_IBASE.
 */
#define IMPLEMENT_IBASE_END						\
  return scfParent ?							\
    scfParent->QueryInterface (iInterfaceID, iVersion) : NULL;		\
}

/// �� ���� �� ��, ��� � IMPLEMENT_IBASE_END
#define IMPLEMENT_EMBEDDED_IBASE_END					\
  IMPLEMENT_IBASE_END

/**
 * ������ IMPLEMENT_INTERFACE ������������ ��� ������������ ���� �����������,
 * ����������� ������� ������� (������������ ������ "������" �� IMPLEMENT_IBASE
 * � IMPLEMENT_IBASE_END).
 */
#define IMPLEMENTS_INTERFACE(Interface)					\
  IMPLEMENTS_INTERFACE_COMMON (Interface, this)

/**
 * IMPLEMENT_EMBEDDED_INTERFACE ����� �� IMPLEMENT_INTERFACE, �� ������������
 * ����� �������������� ��������� ��������� �� ��������� ������.
 */
#define IMPLEMENTS_EMBEDDED_INTERFACE(Interface)			\
  IMPLEMENTS_INTERFACE_COMMON (Interface, (&scf##Interface))

// ������ ������������ �� �������� IMPLEMENTS_XXX_INTERFACE
#define IMPLEMENTS_INTERFACE_COMMON(Interface,Object)			\
  if (scfCompatibleVersion (iVersion, VERSION_##Interface)		\
   && !strcmp (iInterfaceID, #Interface))				\
  {									\
    (Object)->IncRef ();						\
    return static_cast<Interface*> (Object);				\
  }

/**
 * ������ IMPLEMENT_FACTORY ������������ ��� ����������� ������� -
 * ���������� SCF ��������. ��� ������� ���������� ������ ���, �����
 * ������������ ���������� �������� ������ SCF �������.
 */
#define IMPLEMENT_FACTORY(Class)					\
void *Create_##Class (iBase *iParent)					\
{									\
  void *ret = new Class (iParent);					\
  SCF_TRACE (("  %p = new %s ()\n", ret, #Class));			\
  return ret;								\
}

/**
 * ������ DECLARE_FACTORY ������������, ����� �������� �������� �������
 * ���������� SCF �������� � ������, ���� ���� ������� ���������� � ������
 * ������ (����������� IMPLEMENT_FACTORY).
 */
#define DECLARE_FACTORY(Class)  void *Create_##Class (iBase *iParent);

/**
 * ������ �� ����� �������� �������������� �� ������ ������������
 * ����������, ���������� ����� SCF �������. ������ ��������� �� ����
 * ������ ���������� ����� ������ ������������ (��� �������) ��������������
 * �� ���� ������������ ��������� �������.
 */
struct scfClassInfo
{
  /// ������������� ������.
  char *ClassID;
  /// �������� ������.
  char *Description;
  /**
   * ������������ ������ ��������������� �������, �� ������� ������� ������
   * ������. ��� �������� �������� ���������� ������� ���������� ������ ����
   * ������������ ��� ������ ��� ����������� ������� �� ��������.
   */
  char *Dependencies;
  /// �������-��������� �������� ������ ������.
  void *(*Factory) (iBase *iParent);
};

/*
 * ������������� ������� ������������ ��� ����������� ������� ������� ��������
 * ���������� ��� ���� �������������� SCF �������. ����� ������� ������������
 * ��� � ������ �����������, ��� � � ������ ������������ �������� SCF �������.
 */

/**
 * ���������� ������ ������� �������������� �������.
 * ����� ������, �������������� �����-�� SCF ������ ������ ����������
 * ������� �� ������� ���� �������������� �������. �������� LibraryName
 * ������������ ��� ��������������� ����� ������� �������������� ��
 * ������������ ����������; ������ ��� #LibraryName#_GetClassTable.
 */
#define EXPORT_CLASS_TABLE(LibraryName)					\
SCF_EXPORT_FUNCTION scfClassInfo*					\
SCF_EXPORTED_NAME(LibraryName,_GetClassTable)()				\
{									\
  static scfClassInfo ExportClassTable [] =				\
  {

/// �������� ���������� �� �������������� ������ � ������.
#define EXPORT_CLASS(Class, ClassID, Description)			\
    { ClassID, Description, NULL, Create_##Class },

/// �������� ���������� �� �������������� ������ � � ��� ������������ � ������.
#define EXPORT_CLASS_DEP(Class, ClassID, Description, Dependencies)	\
    { ClassID, Description, Dependencies, Create_##Class },

/// ��������� ����������� ������ �������������� �������.
#define EXPORT_CLASS_TABLE_END						\
    { 0, 0, 0, 0 }							\
  };									\
  return ExportClassTable;						\
}

/**
 * ��������������� ������������ SCF ������. ��� ������������� ������������
 * ���������� SCF ������� ���������� �����-�� ������� ���� ����������������
 * ���� ����� � ����������� ������ SCF �������, ��� ��� � ���� ������
 * ����������� ����������� ������� ��� ��� �������� ������������ ����������.
 */
#define REGISTER_STATIC_LIBRARY(LibraryName)				\
  extern "C" scfClassInfo *LibraryName##_GetClassTable ();		\
  class __##LibraryName##_Init						\
  {									\
  public:								\
    __##LibraryName##_Init ()						\
    { scfRegisterClassList (LibraryName##_GetClassTable ()); }		\
  } __##LibraryName##_dummy;

/**
 * ��� ������� ������ �� REGISTER_STATIC_LIBRARY, �� ������������ SCF �����,
 * ������� ������ ���������� ����������� ������ � �������� ����������. ���
 * ���� ������� �������������� ������� (������� EXPORT_CLASS_XXX) �� �����,
 * ���������� � ����� �� ������ �������� ��������� �������� ������
 * REGISTER_STATIC_CLASS.
 */
#define REGISTER_STATIC_CLASS(Class,ClassID,Description)		\
  REGISTER_STATIC_CLASS_DEP (Class,ClassID,Description,NULL);

/**
 * ���� ������ ������� REGISTER_STATIC_CLASS �� ��������� ��������������
 * �������� ��� ������� ������ �� ������� ������������ ������� ������.
 */
#define REGISTER_STATIC_CLASS_DEP(Class,ClassID,Description,Dependency)	\
  extern void *Create_##Class (iBase *);				\
  static scfClassInfo Class##_ClassInfo =				\
  { ClassID, Description, Dependency, Create_##Class };			\
  class __##Class##_Init						\
  {									\
  public:								\
    __##Class##_Init ()							\
    { scfRegisterStaticClass (&Class##_ClassInfo); }			\
  } __##Class##_dummy;

//--------------------------------------- ��������� ���������� �������� -----//

SCF_VERSION (iFactory, 0, 0, 1);

/**
 * iFactory ��� ���������, ������� ������������ ��� �������� �����������
 * �������� ������-���� SCF ������. ��� ������� ������������ ���������
 * iFactory; ���� ������� ����� QueryInterface("iFactory", VERSION_iFactory)
 * �� �� ������ ��������� �� ��������� iFactory, ��������� ��������� �������
 * ������ �� ������, ��� � ������������ ������. ����� �������, �����
 * ����������� ������� �� ������ � �� ���������� ��� (�.�. iFactory ��������
 * � ��������� ���� "���" �������).
 *
 * ����� ������� ������������� ��������� ������ ���������� SCF. �� �� ������
 * ��������� ������� ������� � ����� ����������� (�� ����� ���� ��, �������,
 * ������ ��� ������ �� ��� ���������� ��� ��� �� ���������� ������� ��������
 * ����� ������ �� ���������� ������ SCF). ������ ����� �� ������
 * �������������� ����� ���������� �������� ��������� scfRegisterClass().
 */
struct iFactory : public iBase
{
  /**
   * ������� ��������� ������� ��� �������� ������������ ������ iFactory.
   * ������������ ��������� ���������� ������������� � ������� ����.
   */
  virtual void *CreateInstance () = 0;
  /**
   * ���������� ��������� ������������ ������ ����� ������.
   * ������ �� ����� �������� ���� ������� ���� ���� ��������� �������,
   * ��� ����� ���������� ���� ������� ��� ���� ������ ����� ����������
   * ������ � �������� ���������.
   */
  virtual void TryUnload () = 0;
  /**
   * �������� ������ �������� ������.
   */
  virtual const char *QueryDescription () = 0;
  /**
   * �������� ������ ������������ ������� ������.
   */
  virtual const char *QueryDependencies () = 0;
};

//----------------------------------------------- Client-side functions -----//

/**
 * ������� ������ ��� �������� ���������� SCF ������. ��� ������� ����� 
 * scfCreateInstance � ��������������� ���� ������������� ������� � �������.
 */
#define CREATE_INSTANCE(ClassID,Interface)				\
  (Interface *)scfCreateInstance (ClassID, #Interface, VERSION_##Interface)

/**
 * ������ ��� �������� ������ ��������� �� ��������� ������-���� �������.
 * ��� ������� ����� ������ iBase::QueryInterface � ����������� �������������
 * �������� � ������� ����.
 */
#define QUERY_INTERFACE(Object,Interface)				\
  (Interface *)(Object)->QueryInterface (#Interface, VERSION_##Interface)

/**
 * ��� ������� �������������� ���������� SCF.
 *
 * ��� ������� ����� ������� ���� ���; ����� �������� ��� �����-�� SCF
 * ������ � ����������� ������ �������, ����������� ������� scfRegisterClass.
 *
 * @param iClassList
 *   ���� ������������ ������������ ����������, �������� ������ �����
 *   (����� ������ ������������ ���������� NULL) ����:
 *   <pre>
 *   <SCF �����> = <������������ ����������>[:<�����������>]
 *   </pre>
 *   "SCF �����" - ��� ��� ������, "������������ ����������" ��� �������
 *   �������� ����������� ������������ ���������� (��� �������� � ��� ��������
 *   "lib" � Unix). "�����������" ��� ������ ������������ ������ (�� ��
 *   ��������� ������������ ����������), ������ ��� �������� ������ SCF
 *   �������, ����������� �������.
 */
extern void scfInitialize (const char * const * iClassList = 0);

/**
 * ��� ������� ���������� �� ��������� ������ �� ����� SCF ��������.
 * ���� ���� �� ��� SCF ������ ���� ���������� ����� ���������� �� ��������
 * ������ �� ����, ��� ������� ��� ����� �������� ������������ ����������,
 * � ������� ��������� ���������� ����� ������, ������� ����� ����� ������
 * SCF ������� ����� ������ ���� ������� �������� � ��������� ���������
 * ���������.
 */
extern void scfFinish ();

/**
 * ��������� ������� ������ � ������� SCF �������.
 * �� ������ ������������ ��� ������� ��� �����������, ���� ������� ������
 * �������� ���������� ������ � ���������� ������� ������ � �����������
 * ������� SCF �������.
 */
extern bool scfClassRegistered (const char *iClassID);

/**
 * ������� ��������� ������ � ������� ��������� �� ������������ ��� ���������
 * � ������ ������. ������� ���������� NULL ���� ���� ����� ����� ��
 * ��������������� � ������� SCF �������, ���� ���� ������ �� ������������
 * ������������� ���������, ���� ���� �������������� ��������� �����
 * ����� ������, ������������� � �������������. ��� ����� ������� �����������
 * ������� ������ �������������� �������� scfClassRegistered().
 *
 * @param iClassID
 *   ������������� SCF ������.
 * @param iInterfaceID
 *   ������������� ����������. ���� ����� NULL, ������� ���������� ���������
 *   �� ������� ��������� ������� ������� (iBase). <b>��� ���� ������� ������
 *   ������� ����� �������, ������� �� �������� ��������� ��� ��������
 *   IncRef</b>. ��� ������ ������ QueryInterface ������� ������ �����
 *   �������������, ������� � ������ ��������� ������ ���� ������� �������
 *   ������ ����������� ����������� �� ����.
 * @param iVersion
 *   ������ ���������� (��������������� �������� SCF_VERSION).
 * @return
 *   ��������� �� ����������� ���������.
 */
extern void *scfCreateInstance (const char *iClassID, const char *iInterfaceID,
  int iVersion);

/**
 * ��������� �������� ������.
 * <b>��������:</b> �� ������ ������ ������� ������ ������������ ��� �������
 * ���� ��������� ������� ������� ������, ���� ����� ������ ���� ����������
 * ������. ����� ������� ���������� NULL.
 * @param iClassID
 *   ������������� SCF ������.
 */
extern const char *scfGetClassDescription (const char *iClassID);

/**
 * ��������� ������ ������������ ������.
 * ������ ������ ������������ ������� �� ����������; ���� ���������� SCF
 * �� ������ ������� ������������� � ���������� ������� ���� ������.
 * @param iClassID
 *   ������������� SCF ������.
 */
extern const char *scfGetClassDependencies (const char *iClassID);

/**
 * ��������� ��� �������������� ������������ ���������� (�������������
 * ���������� ������ ������� scfCreateInstance). ���� ������ ��������������,
 * ��� ��� �������������� ������������ ���������� ���������, ��������
 * ��� �������.
 */
extern void scfUnloadUnusedModules ();

/**
 * ���������������� SCF �����, ����������� � ����������� ������������
 * ����������. ��� ������� ��������� ����� � ����������� ������ SCF �������.
 * � ����� ����������� ������������ ���������� ����� ���������� ���������
 * SCF �������, �� ��� ����� ������� ����� ����� ������� ��������� ���.
 */
extern bool scfRegisterClass (const char *iClassID, const char *iLibraryName,
  const char *Dependencies = NULL);

/**
 * ���������������� ���������� ��������� � �������� ���������� SCF �����.
 * ������� ���������� scfRegisterClass, �� ������������� ��� �����������
 * �������. ��� �������� ���������� ��������� �������� REGISTER_STATIC_CLASS,
 * ������� �������� ���������� � ����������� ����� �������.
 */
extern bool scfRegisterStaticClass (scfClassInfo *iClassInfo);

/**
 * ���������������� ����� ����������� �������. ��� ������� ��������
 * scfRegisterStaticClass ��� ������� �������� �������. ��������� �������
 * ������� ����� ���� ClassID ������ NULL.
 *
 * ��� �������� ����� ��������������� �������� SCF_REGISTER_STATIC_LIBRARY,
 * ������� �������� ��� �������.
 */
extern bool scfRegisterClassList (scfClassInfo *iClassInfo);

/**
 * ��� ������� ����� ������������ ��� ������������� ������ �� ����� ����������
 * ���������. ��� ������� ������� ��� ������, ���������������� ���������
 * scfRegisterXXX(), ��� � ������, ��� �������� ���� �������� �������
 * scfInitialize().
 */
extern bool scfUnregisterClass (char *iClassID);

/**
 * ��� ������� ���������, ���� ������ ���������� ���������� � �������������
 * ������������� �������. SCF ���������� ��������� �������� ��������: ������
 * ���������� ���� ������� ����� ������ �����, � ������� � ����� ������ ������
 * ���������� ������ ��� ����� �������������.
 *
 * @param iVersion
 *   ������������� ����� ������ ����������.
 * @param iItfVersion
 *   ����� ������ ����������.
 */
static inline bool scfCompatibleVersion (int iVersion, int iItfVersion)
{
  return ((iVersion & 0xff000000) == (iItfVersion & 0xff000000))
      && ((iVersion & 0x00ffffff) <= (iItfVersion & 0x00ffffff));
}

SCF_VERSION (iSCF, 0, 0, 1);

/**
 * iSCF ��� ���������, �������������� ������ � ������� �������� SCF.
 * ������ SCF ������� �� ������ �������� ������� scfXXX() ��������, ��
 * ������ �������� ��������� �� ��������� iSCF � �������� ����� ����.
 * ��� ������� � ���, ��� ��� ������������ SCF ������ � �����������
 * ������������ ���������� �� �� ����� ����������� �������� �������,
 * ����������� � �������� ��������� (�� ������ ������ � Windows � OS/2).
 */
struct iSCF : public iBase
{
  /// �� ��, ��� � scfClassRegistered ()
  virtual bool ClassRegistered (const char *iClassID) = 0;
  /// �� ��, ��� � scfCreateInstance ()
  virtual void *CreateInstance (const char *iClassID,
    const char *iInterfaceID, int iVersion) = 0;
  /// �� ��, ��� � scfGetClassDescription ()
  virtual const char *GetClassDescription (const char *iClassID) = 0;
  /// �� ��, ��� � scfGetClassDependencies ()
  virtual const char *GetClassDependencies (const char *iClassID) = 0;
  /// �� ��, ��� � scfRegisterClass ()
  virtual bool RegisterClass (const char *iClassID,
    const char *iLibraryName, const char *Dependencies = NULL) = 0;
  /// �� ��, ��� � scfRegisterStaticClass ()
  virtual bool RegisterStaticClass (scfClassInfo *iClassInfo) = 0;
  /// �� ��, ��� � scfRegisterClassList ()
  virtual bool RegisterClassList (scfClassInfo *iClassInfo) = 0;
  /// �� ��, ��� � scfUnregisterClass ()
  virtual bool UnregisterClass (char *iClassID) = 0;
};

//-------------------------------------- ��������-��������� ����������� -----//

/*
 * ������ ��� �������� ������� �� ������������ ����������.
 * �� ��������� ���������� ����� ������������ �������������� ������ ����������.
 * �������� � Windows �����-�� ���������� ��������� �������������� �������
 * ��� __declspec((dllexport)).
 */
#if !defined(SCF_EXPORT_FUNCTION)
#  define SCF_EXPORT_FUNCTION extern "C"
#endif

/*
 * ������ ��� ���������� ���� �������������� �������. ������ "Prefix"
 * ������������ �� ����� ������������ ����������, ������� ��� ���� ���������
 * �� ����� ����� ������ ����� ���� �������. ��� ����� ��� �������������
 * ����������� ������ �������, ��� ��� ����� ����� �� ���������� �������������
 * �������.
 */
#if !defined(SCF_EXPORTED_NAME)
#  define SCF_EXPORTED_NAME(Prefix, Suffix) Prefix ## Suffix
#endif

#endif // __SCF_H__
