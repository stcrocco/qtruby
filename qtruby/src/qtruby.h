/***************************************************************************
                          qtruby.h  -  description
                             -------------------
    begin                : Fri Jul 4 2003
    copyright            : (C) 2003 by Richard Dale
    email                : Richard_Dale@tipitina.demon.co.uk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QTRUBY_H
#define QTRUBY_H

#include <QHash>
#include <smoke/smoke.h>

#include "marshall.h"

#ifndef QT_VERSION_STR
#define QT_VERSION_STR "Unknown"
#endif
#define QTRUBY_VERSION "1.4.10"

inline bool operator==(const Smoke::ModuleIndex& a, const Smoke::ModuleIndex& b) {
	if (a.index == b.index && a.smoke == b.smoke)
		return true;
	else
		return false;
}

inline uint qHash(const Smoke::ModuleIndex& mi) {
	return qHash(mi.index) ^ qHash(mi.smoke);
}

struct MocArgument;

class Q_DECL_EXPORT QtRubySmokeBinding : public SmokeBinding {
public:
	QtRubySmokeBinding(Smoke *s);
	void deleted(Smoke::Index classId, void *ptr);
	bool callMethod(Smoke::Index method, void *ptr, Smoke::Stack args, bool /*isAbstract*/);
	char *className(Smoke::Index classId);
};

struct smokeruby_object {
    bool allocated;
    Smoke *smoke;
    int classId;
    void *ptr;
};

struct TypeHandler {
    const char *name;
    Marshall::HandlerFn fn;
};

extern Q_DECL_EXPORT int do_debug;   // evil
extern Q_DECL_EXPORT VALUE rv_qapp;
extern Q_DECL_EXPORT int object_count;

typedef const char* (*ResolveClassNameFn)(Smoke* smoke, int classId, void * ptr);
typedef void (*ClassCreatedFn)(const char* package, VALUE module, VALUE klass);

struct QtRubyModule {
    const char *name;
    ResolveClassNameFn resolve_classname;
    ClassCreatedFn class_created;
};

// keep this enum in sync with lib/Qt/debug.pm

enum QtDebugChannel {
    qtdb_none = 0x00,
    qtdb_ambiguous = 0x01,
    qtdb_method_missing = 0x02,
    qtdb_calls = 0x04,
    qtdb_gc = 0x08,
    qtdb_virtual = 0x10,
    qtdb_verbose = 0x20
};

extern "C" {
extern Q_DECL_EXPORT VALUE qt_internal_module;
extern Q_DECL_EXPORT VALUE qt_module;

extern Q_DECL_EXPORT VALUE qlistmodel_class;
extern Q_DECL_EXPORT VALUE qmetaobject_class;
extern Q_DECL_EXPORT VALUE qtablemodel_class;
extern Q_DECL_EXPORT VALUE qt_base_class;
extern Q_DECL_EXPORT VALUE qvariant_class;

extern Q_DECL_EXPORT VALUE moduleindex_class;

extern Q_DECL_EXPORT bool application_terminated;
}


extern Q_DECL_EXPORT Smoke::ModuleIndex _current_method;

extern Q_DECL_EXPORT QHash<Smoke*, QtRubyModule> qtruby_modules;
extern Q_DECL_EXPORT QList<Smoke*> smokeList;

extern Q_DECL_EXPORT QHash<QByteArray, Smoke::ModuleIndex *> methcache;
extern Q_DECL_EXPORT QHash<QByteArray, Smoke::ModuleIndex *> classcache;
// Maps from an int id to classname in the form Qt::Widget
extern Q_DECL_EXPORT QHash<Smoke::ModuleIndex, QByteArray*> classname;

extern Q_DECL_EXPORT void install_handlers(TypeHandler *);

extern Q_DECL_EXPORT void smokeruby_mark(void * ptr);
extern Q_DECL_EXPORT void smokeruby_free(void * ptr);
extern Q_DECL_EXPORT VALUE qchar_to_s(VALUE self);

extern Q_DECL_EXPORT smokeruby_object * alloc_smokeruby_object(bool allocated, Smoke * smoke, int classId, void * ptr);
extern Q_DECL_EXPORT void free_smokeruby_object(smokeruby_object * o);
extern Q_DECL_EXPORT smokeruby_object *value_obj_info(VALUE value);
extern Q_DECL_EXPORT void *value_to_ptr(VALUE ruby_value); // ptr on success, null on fail

extern Q_DECL_EXPORT VALUE getPointerObject(void *ptr);
extern Q_DECL_EXPORT void mapPointer(VALUE obj, smokeruby_object *o, Smoke::Index classId, void *lastptr);
extern Q_DECL_EXPORT void unmapPointer(smokeruby_object *, Smoke::Index, void*);

extern Q_DECL_EXPORT const char * resolve_classname(Smoke* smoke, int classId, void * ptr);
extern Q_DECL_EXPORT void rb_str_catf(VALUE self, const char *format, ...) __attribute__ ((format (printf, 2, 3)));

extern Q_DECL_EXPORT VALUE findMethod(VALUE self, VALUE c_value, VALUE name_value);
extern Q_DECL_EXPORT VALUE findAllMethods(int argc, VALUE * argv, VALUE self);
extern Q_DECL_EXPORT VALUE findAllMethodNames(VALUE self, VALUE result, VALUE classid, VALUE flags_value);

extern Q_DECL_EXPORT QByteArray* find_cached_selector(int argc, VALUE * argv, VALUE klass, const char * methodName);
extern Q_DECL_EXPORT VALUE method_missing(int argc, VALUE * argv, VALUE self);
extern Q_DECL_EXPORT VALUE class_method_missing(int argc, VALUE * argv, VALUE klass);
extern Q_DECL_EXPORT QList<MocArgument*> get_moc_arguments(Smoke* smoke, const char * typeName, QList<QByteArray> methodTypes);

extern Q_DECL_EXPORT void * construct_copy(smokeruby_object *o);

extern "C"
{
extern Q_DECL_EXPORT VALUE mapObject(VALUE self, VALUE obj);
extern Q_DECL_EXPORT VALUE qobject_metaobject(VALUE self);
extern Q_DECL_EXPORT VALUE set_obj_info(const char * className, smokeruby_object * o);
extern Q_DECL_EXPORT VALUE cast_object_to(VALUE self, VALUE object, VALUE new_klass);
extern Q_DECL_EXPORT VALUE kross2smoke(VALUE self, VALUE krobject, VALUE new_klass);
extern Q_DECL_EXPORT VALUE smoke2kross(VALUE self, VALUE sobj);
extern Q_DECL_EXPORT VALUE qvariant_value(VALUE self, VALUE variant_value_klass, VALUE variant_value);
extern Q_DECL_EXPORT VALUE qvariant_from_value(int argc, VALUE * argv, VALUE self);
extern Q_DECL_EXPORT const char* get_VALUEtype(VALUE ruby_value);
extern Q_DECL_EXPORT VALUE prettyPrintMethod(Smoke::Index id);
}

#endif
