/*
    Shared Class Facility (SCF)
    Copyright (C) 1999 by Andrew Zabolotny

    Base SCF definitions, macros and interfaces.
*/

#ifndef __SCF_H__
#define __SCF_H__

#include "scf/scfdefs.h"

/**
 * Макрос для отладочной печати: Добавьте #define SCF_DEBUG в начале
 * интересующего Вас модуля и перекомпилируйте; при этом все основные
 * операции SCF будут печатать отладочную информацию.
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
 * Макрос для печати адреса, откуда вызвана процедура.
 * Работает только с GCC >= 2.8.0.
 */
#if (__GNUC__ >= 2) && (__GNUC_MINOR__ >= 8)
#  define SCF_PRINT_CALL_ADDRESS					\
   printf ("  Called from address %p\n", __builtin_return_address (0));
#else
#  define SCF_PRINT_CALL_ADDRESS
#endif

/// Макрос для построения номера версии интерфейса.
#define SCF_CONSTRUCT_VERSION(Major,Minor,Micro)			\
  ((Major << 24) | (Minor << 16) | Micro)

/**
 * SCF_VERSION может быть использован как более краткий метод задания
 * номера версии интерфейса; Вы должны указать имя интерфейса и три
 * составные части (major/minor/micro) номера версии интерфейса.
 * Например:
 * <pre>
 * SCF_VERSION (iSomething, 0, 0, 1);
 * struct iSomething : public iBase
 * {
 *   ...
 * };
 * </pre>
 * Приведенный выше пример определяет интерфейс iSomething версии 0.0.1.
 */
#define SCF_VERSION(Name,Major,Minor,Micro)				\
  const int VERSION_##Name = SCF_CONSTRUCT_VERSION (Major, Minor, Micro)

SCF_VERSION (iBase, 0, 0, 1);

/**
 * iBase является базовым интерфейсом; все остальные интерфейсы должны
 * быть производны от него или его производных. Это позволяет работать
 * с основными свойствами обьектов SCF независимо от их реально
 * заданного интерфейса.
 */
struct iBase
{
  /**
   * Увеличить счетчик ссылок на данный обьект.
   * Каждый раз, когда мы копируем указатель на любой обьект для
   * длительного хранения, необходимо вызывать эту функцию для того,
   * чтобы обьект знал о том, что он все еще кому-то нужен. При этом
   * он не допустит своего разрушения до того, как счетчик ссылок
   * станет равным нулю.
   */
  virtual void IncRef () = 0;
  /**
   * Уменьшить счетчик ссылок на данный обьект.
   * Когда мы завершаем работу с некоторым обьектом, вместо деструктора
   * необходимо вызвать эту функцию. При достижении счетчиком ссылок нуля
   * обьект автоматически саморазрушится. Использование этого механизма
   * гарантирует недопущение разрушения обьекта до того, как он перестает
   * быть нужным.
   */
  virtual void DecRef () = 0;
  /**
   * Запросить один из интерфейсов поддерживаемых этим обьектом.
   * При этом в качестве параметра передается идентификатор интерфейса
   * (в виде строки) и его версия. Если обьект обладает запрошенным
   * интерфейсом совместимой версии (критерий совместимости определен
   * функцией scfCompatibleVersion), QueryInterface возвращает указатель
   * на этот интерфейс, иначе функция возвращает NULL. Для удобства
   * использование этой функции не рекомендуется, взамен предлагается
   * макрос QUERY_INTERFACE.
   */
  virtual void *QueryInterface (const char *iInterfaceID, int iVersion) = 0;
};

/**
 * Этот макрос можно использовать внутри определения класса,
 * поддерживающего интерфейс SCF для определения базовых функций
 * унаследованных от класса iBase.
 */
#define DECLARE_IBASE							\
  DECLARE_EMBEDDED_IBASE (iBase)

/**
 * DECLARE_EMBEDDED_IBASE используется для определения базовых функций
 * унаследованных от класса iBase внутри вложенного класса который также
 * имеет SCF интерфейс. OuterClass это тип охватывающего класса.
 */
#define DECLARE_EMBEDDED_IBASE(OuterClass)				\
public:									\
  int scfRefCount;		/* Reference counter */			\
  OuterClass *scfParent;	/* The parent object */			\
  virtual void IncRef ();						\
  virtual void DecRef ();						\
  virtual void *QueryInterface (const char *iInterfaceID, int iVersion)

/**
 * Макрос CONSTRUCT_IBASE должен вызываться из конструктора SCF обьекта
 * (но не из конструкторов вложенных классов). Обычно каждый такой
 * конструктор имеет параметр типа iBase* (передаваемый в конструктор
 * функцией scfCreateInstance), который и передается в качетсве аргумента
 * этому макросу. Макрос попросту обнуляет счетчик ссылок и инициализирует
 * указатель на обьект-родитель.
 */
#define CONSTRUCT_IBASE(Parent)						\
  scfRefCount = 1; if ((scfParent = Parent)) scfParent->IncRef();

/**
 * Макрос CONSTRUCT_EMBEDDED_IBASE вызывается из конструктора экспортируемого
 * класса, имеющего вложенные SCF интерфейсы. Макрос обнуляет счетчик ссылок
 * вложенного обьекта и устанавливает его указатель на родительский обьект
 * на себя.
 */
#define CONSTRUCT_EMBEDDED_IBASE(Interface)				\
  Interface.scfRefCount = 0; Interface.scfParent = this;

/**
 * Этот макрос облегчает определение базовой функциональности (унаследованной
 * от iBase). Вместо того, чтобы определять все эти IncRef(), DecRef() и
 * так далее можно просто вызвать IMPLEMENT_IBASE с одним аргументом -
 * типом класса.
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
 * IMPLEMENT_EMBEDDED_IBASE делает то же, что и IMPLEMENT_IBASE,
 * но для вложенных интерфейсов. Он отличается от него тем, что
 * при уменьшении его счетчика ссылок до нуля ему не нужно
 * самоудалять себя (так как это будет сделано при удалении
 * родительского обьекта).
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
 * Этот макрос используется для окончания обьявления IMPLEMENT_IBASE.
 */
#define IMPLEMENT_IBASE_END						\
  return scfParent ?							\
    scfParent->QueryInterface (iInterfaceID, iVersion) : NULL;		\
}

/// По сути то же, что и IMPLEMENT_IBASE_END
#define IMPLEMENT_EMBEDDED_IBASE_END					\
  IMPLEMENT_IBASE_END

/**
 * Макрос IMPLEMENT_INTERFACE используется для перечисления всех интерфейсов,
 * реализуемым текущим классом (используется внутри "скобок" из IMPLEMENT_IBASE
 * и IMPLEMENT_IBASE_END).
 */
#define IMPLEMENTS_INTERFACE(Interface)					\
  IMPLEMENTS_INTERFACE_COMMON (Interface, this)

/**
 * IMPLEMENT_EMBEDDED_INTERFACE похож на IMPLEMENT_INTERFACE, но используется
 * когда экспортируемый интерфейс определен во вложенном классе.
 */
#define IMPLEMENTS_EMBEDDED_INTERFACE(Interface)			\
  IMPLEMENTS_INTERFACE_COMMON (Interface, (&scf##Interface))

// Макрос используемый из макросов IMPLEMENTS_XXX_INTERFACE
#define IMPLEMENTS_INTERFACE_COMMON(Interface,Object)			\
  if (scfCompatibleVersion (iVersion, VERSION_##Interface)		\
   && !strcmp (iInterfaceID, #Interface))				\
  {									\
    (Object)->IncRef ();						\
    return static_cast<Interface*> (Object);				\
  }

/**
 * Макрос IMPLEMENT_FACTORY используется для определения функции -
 * генератора SCF обьектов. Эта функция вызывается всякий раз, когда
 * пользователь заказывает создание нового SCF обьекта.
 */
#define IMPLEMENT_FACTORY(Class)					\
void *Create_##Class (iBase *iParent)					\
{									\
  void *ret = new Class (iParent);					\
  SCF_TRACE (("  %p = new %s ()\n", ret, #Class));			\
  return ret;								\
}

/**
 * Макрос DECLARE_FACTORY используется, чтобы обьявить прототип функции
 * генератора SCF обьектов в случае, если сама функция определена в другом
 * модуле (посредством IMPLEMENT_FACTORY).
 */
#define DECLARE_FACTORY(Class)  void *Create_##Class (iBase *iParent);

/**
 * Массив из таких структур экспортируется из каждой динамической
 * библиотеки, содержащую набор SCF классов. Обычно указатель на этот
 * массив получается путем вызова единственной (как правило) экспортируемой
 * из этих динамических библиотек функции.
 */
struct scfClassInfo
{
  /// Идентификатор класса.
  char *ClassID;
  /// Описание класса.
  char *Description;
  /**
   * Опциональный список идентификаторов классов, от которых зависит данный
   * модуль. При загрузки большого количества модулей приложение должно само
   * использовать эту строку для определения порядка их загрузки.
   */
  char *Dependencies;
  /// Функция-генератор обьектов такого класса.
  void *(*Factory) (iBase *iParent);
};

/*
 * Нижеследующие макросы используются для определения таблицы которая содержит
 * информацию обо всех экспортируемых SCF классах. Такие таблицы используются
 * как в случае статической, так и в случае динамической линковки SCF модулей.
 */

/**
 * Обьявление начала таблицы экспортируемых классов.
 * Любой модуль, экспортирующий какие-то SCF классы должен определить
 * таблицу со списком всех экспортируемых классов. Параметр LibraryName
 * используется для конструирования имени функции экспортируемой из
 * динамической библиотеки; обычно это #LibraryName#_GetClassTable.
 */
#define EXPORT_CLASS_TABLE(LibraryName)					\
SCF_EXPORT_FUNCTION scfClassInfo*					\
SCF_EXPORTED_NAME(LibraryName,_GetClassTable)()				\
{									\
  static scfClassInfo ExportClassTable [] =				\
  {

/// Добавить информацию об экспортируемом классе в список.
#define EXPORT_CLASS(Class, ClassID, Description)			\
    { ClassID, Description, NULL, Create_##Class },

/// Добавить информацию об экспортируемом классе и о его зависимостях в список.
#define EXPORT_CLASS_DEP(Class, ClassID, Description, Dependencies)	\
    { ClassID, Description, Dependencies, Create_##Class },

/// Закончить определение списка экспортируемых классов.
#define EXPORT_CLASS_TABLE_END						\
    { 0, 0, 0, 0 }							\
  };									\
  return ExportClassTable;						\
}

/**
 * Авторегистрация статического SCF модуля. При использовании статического
 * связывания SCF модулей необходимо каким-то образом явно зарегистрировать
 * этот класс в центральный реестр SCF классов, так как в этом случае
 * отсутствует возможность сделать это при загрузке динамической библиотеки.
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
 * Эта функция похожа на REGISTER_STATIC_LIBRARY, но регистрирует SCF класс,
 * который всегда использует статическую связку с основной программой. При
 * этом таблица экспортируемых классов (макросы EXPORT_CLASS_XXX) не нужна,
 * достаточно в одном из файлов головной программы вызывать макрос
 * REGISTER_STATIC_CLASS.
 */
#define REGISTER_STATIC_CLASS(Class,ClassID,Description)		\
  REGISTER_STATIC_CLASS_DEP (Class,ClassID,Description,NULL);

/**
 * Этот макрос подобен REGISTER_STATIC_CLASS но принимает дополнительный
 * аргумент для задания строки со списком зависимостей данного класса.
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

//--------------------------------------- Интерфейс генератора обьектов -----//

SCF_VERSION (iFactory, 0, 0, 1);

/**
 * iFactory это интерфейс, который используется для создания экземпляров
 * обьектов какого-либо SCF класса. Все обьекты поддерживают интерфейс
 * iFactory; если вызвать метод QueryInterface("iFactory", VERSION_iFactory)
 * то он вернет указатель на интерфейс iFactory, способный создавать обьекты
 * такого же класса, как и опрашиваемый обьект. Таким образом, можно
 * клонировать обьекты не вникая в их конкретный тип (т.е. iFactory является
 * в некотором роде "ДНК" обьекта).
 *
 * Такие обьекты автоматически создаются внутри библиотеки SCF. Вы не можете
 * создавать вручную обьекты с таким интерфейсом (на самом деле Вы, конечно,
 * можете это делать но это бесполезно так как не существует способа добавить
 * такой обьект во внутренний реестр SCF). Вместо этого Вы должны
 * регистрировать новые генераторы обьектов используя scfRegisterClass().
 */
struct iFactory : public iBase
{
  /**
   * Создать экземпляр обьекта для которого предназначен данный iFactory.
   * Возвращаемый результат необходимо преобразовать к нужному типу.
   */
  virtual void *CreateInstance () = 0;
  /**
   * Попытаться выгрузить динамический модуль этого класса.
   * Модуль не будет выгружен если имеется хоть один экземпляр обьекта,
   * чей класс реализован этим модулем или если данный класс статически
   * связан в основную программу.
   */
  virtual void TryUnload () = 0;
  /**
   * Получить строку описания класса.
   */
  virtual const char *QueryDescription () = 0;
  /**
   * Получить строку зависимостей данного класса.
   */
  virtual const char *QueryDependencies () = 0;
};

//----------------------------------------------- Client-side functions -----//

/**
 * Удобный макрос для создания экземпляра SCF класса. Это простой вызов 
 * scfCreateInstance с преобразованием типа возвращаемого обьекта к нужному.
 */
#define CREATE_INSTANCE(ClassID,Interface)				\
  (Interface *)scfCreateInstance (ClassID, #Interface, VERSION_##Interface)

/**
 * Макрос для удобства опроса указателя на интерфейс какого-либо обьекта.
 * Это простой вызов метода iBase::QueryInterface с приведением возвращаемого
 * значения к нужному типу.
 */
#define QUERY_INTERFACE(Object,Interface)				\
  (Interface *)(Object)->QueryInterface (#Interface, VERSION_##Interface)

/**
 * Эта функция инициализирует библиотеку SCF.
 *
 * Эту функцию можно вызвать лишь раз; чтобы добавить еще какие-то SCF
 * классы в центральный реестр классов, используйте функцию scfRegisterClass.
 *
 * @param iClassList
 *   Если используется динамическое связывание, содержит список строк
 *   (конец списка определяется указателем NULL) вида:
 *   <pre>
 *   <SCF класс> = <динамическая библиотека>[:<зависимости>]
 *   </pre>
 *   "SCF класс" - это имя класса, "динамическая библиотека" это базовое
 *   название динамически подгружаемой библиотеки (без суффикса и без префикса
 *   "lib" в Unix). "Зависимости" это список зависимостей класса (за их
 *   обработку ответственно приложение), обычно это названия других SCF
 *   классов, разделенных запятой.
 */
extern void scfInitialize (const char * const * iClassList = 0);

/**
 * Эта функция вызывается по окончании работы со всеми SCF классами.
 * Даже если не все SCF классы были уничтожены путем уменьшения их счетчика
 * ссылок до нуля, эта функция все равно выгрузит динамические библиотеки,
 * в которых находится реализация этого класса, поэтому любой вызов метода
 * SCF обьекта после вызова этой функции приведет к аварийной остановке
 * программы.
 */
extern void scfFinish ();

/**
 * Проверить наличие класса в реестре SCF классов.
 * Вы можете использовать эту функцию для определения, если причина ошибки
 * создания экземпляра класса в отсутствии данного класса в центральном
 * реестре SCF классов.
 */
extern bool scfClassRegistered (const char *iClassID);

/**
 * Создать экземпляр класса и вернуть указатель на интересующий нас интерфейс
 * в данном классе. Функция возвращает NULL либо если такой класс не
 * зарегистрирован в реестре SCF классов, либо если обьект не поддерживает
 * запрашиваемый интерфейс, либо если поддерживаемый интерфейс имеет
 * номер версии, несовместимый с запрашиваемым. Для более точного определения
 * причины ошибки воспользуйтесь функцией scfClassRegistered().
 *
 * @param iClassID
 *   Идентификатор SCF класса.
 * @param iInterfaceID
 *   Идентификатор интерфейса. Если равно NULL, функция возвращает указатель
 *   на базовый интерфейс данного обьекта (iBase). <b>При этом счетчик ссылок
 *   обьекта будет нулевым, поэтому не забудьте увеличить его функцией
 *   IncRef</b>. При вызове метода QueryInterface счетчик ссылок также
 *   увеличивается, поэтому в случае успешного вызова этой функции счетчик
 *   ссылок искуственно увеличивать не надо.
 * @param iVersion
 *   Версия интерфейса (сформированного макросом SCF_VERSION).
 * @return
 *   Указатель на запрошенный интерфейс.
 */
extern void *scfCreateInstance (const char *iClassID, const char *iInterfaceID,
  int iVersion);

/**
 * Запросить описание класса.
 * <b>ВНИМАНИЕ:</b> На момент вызова функции должен существовать как минимум
 * один экземпляр обьекта данного класса, либо класс должен быть статически
 * связан. Иначе функция возвращает NULL.
 * @param iClassID
 *   Идентификатор SCF класса.
 */
extern const char *scfGetClassDescription (const char *iClassID);

/**
 * Запросить список зависимостей класса.
 * Формат строки зависимостей зависит от приложения; сама библиотека SCF
 * не делает никаких предположений о конкретном формате этой строки.
 * @param iClassID
 *   Идентификатор SCF класса.
 */
extern const char *scfGetClassDependencies (const char *iClassID);

/**
 * Выгрузить все неиспользуемые динамические библиотеки (автоматически
 * вызывается внутри функции scfCreateInstance). Если хотите удостовериться,
 * что все неиспользуемые динамические библиотеки выгружены, вызовите
 * эту функцию.
 */
extern void scfUnloadUnusedModules ();

/**
 * Зарегистрировать SCF класс, находящийся в динамически подгружаемой
 * библиотеки. Эта функция добавляет класс в центральный реестр SCF классов.
 * В одной динамически подгружаемой библиотеке могут находиться несколько
 * SCF классов, но для этого функцию нужно будет вызвать несколько раз.
 */
extern bool scfRegisterClass (const char *iClassID, const char *iLibraryName,
  const char *Dependencies = NULL);

/**
 * Зарегистрировать статически связанный с основной программой SCF класс.
 * Функция аналогична scfRegisterClass, но предназначена для статических
 * классов. Для удобства существует семейство макросов REGISTER_STATIC_CLASS,
 * которые упрощают обьявление и регистрацию таких классов.
 */
extern bool scfRegisterStaticClass (scfClassInfo *iClassInfo);

/**
 * Зарегистрировать набор статических классов. Эта функция вызывает
 * scfRegisterStaticClass для каждого элемента массива. Последний элемент
 * массива имеет поле ClassID равным NULL.
 *
 * Для простоты можно воспользоваться макросом SCF_REGISTER_STATIC_LIBRARY,
 * который вызывает эту функцию.
 */
extern bool scfRegisterClassList (scfClassInfo *iClassInfo);

/**
 * Эту функцию можно использовать для дерегистрации класса во время выполнения
 * программы. Эта функция удаляет как классы, регистрированные функциями
 * scfRegisterXXX(), так и классы, чьи описания были переданы функции
 * scfInitialize().
 */
extern bool scfUnregisterClass (char *iClassID);

/**
 * Эта функция проверяет, если версия интерфейса совместима с запрашиваемой
 * пользователем версией. SCF использует следующий критерий проверки: версии
 * совместимы если старший номер версии равен, а младший и микро номера версии
 * интерфейса больше или равны запрашиваемым.
 *
 * @param iVersion
 *   Запрашиваемый номер версии интерфейса.
 * @param iItfVersion
 *   Номер версии интерфейса.
 */
static inline bool scfCompatibleVersion (int iVersion, int iItfVersion)
{
  return ((iVersion & 0xff000000) == (iItfVersion & 0xff000000))
      && ((iVersion & 0x00ffffff) <= (iItfVersion & 0x00ffffff));
}

SCF_VERSION (iSCF, 0, 0, 1);

/**
 * iSCF это интерфейс, обеспечивающий доступ к базовым функциям SCF.
 * Модули SCF классов не должны вызывать функции scfXXX() напрямую, но
 * должны получить указатель на интерфейс iSCF и работать через него.
 * Это связано с тем, что при расположении SCF модуля в динамически
 * подгружаемой библиотеке он не имеет возможность вызывать функции,
 * находящиеся в основной программе (во всяком случае в Windows и OS/2).
 */
struct iSCF : public iBase
{
  /// То же, что и scfClassRegistered ()
  virtual bool ClassRegistered (const char *iClassID) = 0;
  /// То же, что и scfCreateInstance ()
  virtual void *CreateInstance (const char *iClassID,
    const char *iInterfaceID, int iVersion) = 0;
  /// То же, что и scfGetClassDescription ()
  virtual const char *GetClassDescription (const char *iClassID) = 0;
  /// То же, что и scfGetClassDependencies ()
  virtual const char *GetClassDependencies (const char *iClassID) = 0;
  /// То же, что и scfRegisterClass ()
  virtual bool RegisterClass (const char *iClassID,
    const char *iLibraryName, const char *Dependencies = NULL) = 0;
  /// То же, что и scfRegisterStaticClass ()
  virtual bool RegisterStaticClass (scfClassInfo *iClassInfo) = 0;
  /// То же, что и scfRegisterClassList ()
  virtual bool RegisterClassList (scfClassInfo *iClassInfo) = 0;
  /// То же, что и scfUnregisterClass ()
  virtual bool UnregisterClass (char *iClassID) = 0;
};

//-------------------------------------- Системно-зависимые определения -----//

/*
 * Макрос для экспорта функции из динамической библиотеки.
 * На некоторых платформах может понадобиться переопределить данное обьявление.
 * Например в Windows зачем-то необходимо обьявлять экспортируемые функции
 * как __declspec((dllexport)).
 */
#if !defined(SCF_EXPORT_FUNCTION)
#  define SCF_EXPORT_FUNCTION extern "C"
#endif

/*
 * Макрос для построения имен экспортируемых функций. Обычно "Prefix"
 * производится от имени динамической библиотеки, поэтому для всех библиотек
 * мы будем иметь разные имена этих функций. Это важно при использовании
 * статической связки модулей, так как иначе могли бы возникнуть повторяющиеся
 * символы.
 */
#if !defined(SCF_EXPORTED_NAME)
#  define SCF_EXPORTED_NAME(Prefix, Suffix) Prefix ## Suffix
#endif

#endif // __SCF_H__
