/***************************************************************************
                          akonadihandlers.cpp  -  Akonadi specific marshallers
                             -------------------
    begin                : Thurs May 29 2008
    copyright            : (C) 2008 by Richard Dale
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

#include <ruby.h>

#include <qtruby.h>
#include <smokeruby.h>

#include <akonadi/agentinstance.h>
#include <akonadi/agenttype.h>
#include <akonadi/attribute.h>
#include <akonadi/collection.h>
#include <akonadi/item.h>
#include <akonadi/job.h>

template <class Item, class ItemList, const char *ItemSTR >
void marshall_ItemList(Marshall *m) {
	switch(m->action()) {
		case Marshall::FromVALUE:
		{
			VALUE list = *(m->var());
			if (TYPE(list) != T_ARRAY) {
				m->item().s_voidp = 0;
				break;
			}

			int count = RARRAY(list)->len;
			ItemList *cpplist = new ItemList;
			long i;
			for(i = 0; i < count; i++) {
				VALUE item = rb_ary_entry(list, i);
				// TODO do type checking!
				smokeruby_object *o = value_obj_info(item);
				if(!o || !o->ptr)
					continue;
				void *ptr = o->ptr;
				ptr = o->smoke->cast(
					ptr,				// pointer
					o->classId,				// from
		    		o->smoke->idClass(ItemSTR).index	// to
				);
				cpplist->append((Item*)ptr);
			}

			m->item().s_voidp = cpplist;
			m->next();

			if (!m->type().isConst()) {
				rb_ary_clear(list);
	
				for(int i = 0; i < cpplist->size(); ++i ) {
					VALUE obj = getPointerObject( (void*) cpplist->at(i) );
					rb_ary_push(list, obj);
				}
			}

			if (m->cleanup()) {
				delete cpplist;
			}
		}
		break;
      
		case Marshall::ToVALUE:
		{
			ItemList *valuelist = (ItemList*)m->item().s_voidp;
			if(!valuelist) {
				*(m->var()) = Qnil;
				break;
			}

			VALUE av = rb_ary_new();
			int ix = m->smoke()->idClass(ItemSTR).index;
			const char * className = m->smoke()->binding->className(ix);

			for (int i=0; i<valuelist->size(); ++i) {
				void *p = (void*) valuelist->at(i);

				if (m->item().s_voidp == 0) {
					*(m->var()) = Qnil;
					break;
				}

				VALUE obj = getPointerObject(p);
				if (obj == Qnil) {
					smokeruby_object  * o = alloc_smokeruby_object(false, m->smoke(), ix, p);
					obj = set_obj_info(className, o);
				}
			
				rb_ary_push(av, obj);
			}

			*(m->var()) = av;
			m->next();

			if (m->cleanup()) {
				delete valuelist;
			}
		}
		break;

		default:
			m->unsupported();
		break;
   }
}

#define DEF_LIST_MARSHALLER(ListIdent,ItemList,Item) namespace { char ListIdent##STR[] = #Item; }  \
        Marshall::HandlerFn marshall_##ListIdent = marshall_ItemList<Item,ItemList,ListIdent##STR>;

DEF_LIST_MARSHALLER( AkonadiJobList, QList<Akonadi::Job*>, Akonadi::Job )
DEF_LIST_MARSHALLER( AkonadiAttributeList, QList<Akonadi::Attribute*>, Akonadi::Attribute )


template <class Item, class ItemList, const char *ItemSTR >
void marshall_ValueListItem(Marshall *m) {
	switch(m->action()) {
		case Marshall::FromVALUE:
		{
			VALUE list = *(m->var());
			if (TYPE(list) != T_ARRAY) {
				m->item().s_voidp = 0;
				break;
			}
			int count = RARRAY(list)->len;
			ItemList *cpplist = new ItemList;
			long i;
			for(i = 0; i < count; i++) {
				VALUE item = rb_ary_entry(list, i);
				// TODO do type checking!
				smokeruby_object *o = value_obj_info(item);

				if (!o || !o->ptr)
					continue;
				
				void *ptr = o->ptr;
				ptr = o->smoke->cast(
					ptr,				// pointer
					o->classId,				// from
					o->smoke->idClass(ItemSTR).index	        // to
				);
				cpplist->append(*(Item*)ptr);
			}

			m->item().s_voidp = cpplist;
			m->next();

			if (!m->type().isConst()) {
				rb_ary_clear(list);
				for(int i=0; i < cpplist->size(); ++i) {
					VALUE obj = getPointerObject((void*)&(cpplist->at(i)));
					rb_ary_push(list, obj);
				}
			}

			if (m->cleanup()) {
				delete cpplist;
			}
		}
		break;
      
		case Marshall::ToVALUE:
		{
			ItemList *valuelist = (ItemList*)m->item().s_voidp;
			if(!valuelist) {
				*(m->var()) = Qnil;
				break;
			}

			VALUE av = rb_ary_new();

			int ix = m->smoke()->idClass(ItemSTR).index;
			const char * className = m->smoke()->binding->className(ix);

			for(int i=0; i < valuelist->size() ; ++i) {
				void *p = (void *) &(valuelist->at(i));

				if(m->item().s_voidp == 0) {
				*(m->var()) = Qnil;
				break;
				}

				VALUE obj = getPointerObject(p);
				if(obj == Qnil) {
					smokeruby_object  * o = alloc_smokeruby_object(	false, 
																	m->smoke(), 
																	m->smoke()->idClass(ItemSTR).index, 
																	p );
					obj = set_obj_info(className, o);
				}
		
				rb_ary_push(av, obj);
			}

			*(m->var()) = av;
			m->next();

			if (m->cleanup()) {
				delete valuelist;
			}

		}
		break;
      
		default:
			m->unsupported();
		break;
	}
}

#define DEF_VALUELIST_MARSHALLER(ListIdent,ItemList,Item) namespace { char ListIdent##STR[] = #Item; }  \
        Marshall::HandlerFn marshall_##ListIdent = marshall_ValueListItem<Item,ItemList,ListIdent##STR>;

DEF_VALUELIST_MARSHALLER( AkonadiAgentInstanceList, QList<Akonadi::AgentInstance>, Akonadi::AgentInstance )
DEF_VALUELIST_MARSHALLER( AkonadiAgentTypeList, QList<Akonadi::AgentType>, Akonadi::AgentType )
DEF_VALUELIST_MARSHALLER( AkonadiCollectionList, QList<Akonadi::Collection>, Akonadi::Collection )
DEF_VALUELIST_MARSHALLER( AkonadiItemList, QList<Akonadi::Item>, Akonadi::Item )

TypeHandler Akonadi_handlers[] = {
    { "Akonadi::AgentInstance::List", marshall_AkonadiAgentInstanceList },
    { "Akonadi::AgentType::List", marshall_AkonadiAgentTypeList },
    { "Akonadi::Attribute::List", marshall_AkonadiAttributeList },
    { "Akonadi::Collection::List", marshall_AkonadiCollectionList },
    { "Akonadi::Collection::List&", marshall_AkonadiCollectionList },
    { "Akonadi::Item::List", marshall_AkonadiItemList },
    { "Akonadi::Item::List&", marshall_AkonadiItemList },
    { "Akonadi::Job::List", marshall_AkonadiJobList },
    { "Akonadi::Job::List&", marshall_AkonadiJobList },
    { 0, 0 }
};