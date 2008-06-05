/***************************************************************************
                          kdehandlers.cpp  -  KDE specific marshallers
                             -------------------
    begin                : Sun Sep 28 2003
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

#include <ruby.h>

#include <qtruby.h>
#include <smokeruby.h>

#include <kdeversion.h>
#include <kcoreconfigskeleton.h>
#include <ktrader.h>
#include <kservicegroup.h>
#include <kservice.h>
#include <ksycocatype.h>
#include <kmainwindow.h>
#include <kfile.h>
#include <kfileitem.h>
#include <kurl.h>
#include <kaction.h>
#include <khtml_part.h>
#include <knewstuff2/core/entry.h>
#include <kparts/plugin.h>
#include <kaboutdata.h>
#include <karchive.h>
#include <kconfigskeleton.h>
#include <kplugininfo.h>
#include <kmountpoint.h>
#include <kio/jobclasses.h>

#include <kmultitabbar.h>
#include <kdatatool.h>
#include <kuser.h>
#include <ktoolbar.h>
#include <kio/copyjob.h>

const char*
resolve_classname_kde(Smoke* smoke, int classId, void* ptr)
{
	if (smoke->isDerivedFromByName(smoke->classes[classId].className, "KArchiveEntry")) {
		KArchiveEntry * entry = (KArchiveEntry *) smoke->cast(ptr, classId, smoke->idClass("KArchiveEntry").index);
		if (entry->isDirectory()) {
			return "KDE::ArchiveDirectory";
		} else {
			return "KDE::ArchiveFile";
		}
	}
	
	return smoke->binding->className(classId);
}

#if defined (__i386__) && defined (__GNUC__) && __GNUC__ >= 2
#  define BREAKPOINT { __asm__ __volatile__ ("int $03"); }
#else
#  define BREAKPOINT { fprintf(stderr, "hit ctrl-c\n"); int b = 0; while (b == 0) { ; } }
#endif


void marshall_KServicePtr(Marshall *m) {
	switch(m->action()) {
	case Marshall::FromVALUE: 
		{
		}
		break;
	case Marshall::ToVALUE: 
		{
		KSharedPtr<KService> *ptr = new KSharedPtr<KService>(*(KSharedPtr<KService>*)m->item().s_voidp);
	    if(ptr == 0) {
		*(m->var()) = Qnil;
		break;
	    }
	    KService * service = ptr->data();
	    
		VALUE obj = getPointerObject(service);
		if(obj == Qnil) {
		    smokeruby_object  * o = ALLOC(smokeruby_object);
		    o->smoke = m->smoke();
		    o->classId = m->smoke()->idClass("KService").index;
		    o->ptr = service;
		    o->allocated = true;
		    obj = set_obj_info("KDE::Service", o);
		}

	    *(m->var()) = obj;		
	    
// 		if(m->cleanup())
// 		;
		}
		break;
	default:
		m->unsupported();
		break;
	}
}

void marshall_KSharedConfigPtr(Marshall *m) {
	switch(m->action()) {
	case Marshall::FromVALUE: 
		{
			VALUE config = *(m->var());
			smokeruby_object *o = value_obj_info(config);
			m->item().s_voidp = new KSharedConfigPtr((KSharedConfig*) o->ptr);
			m->next();
			// delete (KSharedConfigPtr*) m->item().s_voidp;
		}
		break;
	case Marshall::ToVALUE: 
		{
		KSharedPtr<KSharedConfig> *ptr = new KSharedPtr<KSharedConfig>(*(KSharedPtr<KSharedConfig>*)m->item().s_voidp);
	    if(ptr == 0) {
		*(m->var()) = Qnil;
		break;
	    }
	    KSharedConfig * config = ptr->data();
	    
		VALUE obj = getPointerObject(config);
		if(obj == Qnil) {
		    smokeruby_object  * o = ALLOC(smokeruby_object);
		    o->smoke = m->smoke();
		    o->classId = m->smoke()->idClass("KSharedConfig").index;
		    o->ptr = config;
		    o->allocated = true;
		    obj = set_obj_info("KDE::SharedConfig", o);
		}

	    *(m->var()) = obj;		
	    
		if(m->cleanup())
		;
		}
		break;
	default:
		m->unsupported();
		break;
	}
}

void marshall_KServiceList(Marshall *m) {
	switch(m->action()) {
	case Marshall::FromVALUE: 
		{
		}
		break;
	case Marshall::ToVALUE: 
		{
	    KService::List *offerList = (KService::List*)m->item().s_voidp;
	    if(!offerList) {
		*(m->var()) = Qnil;
		break;
	    }

	    VALUE av = rb_ary_new();

	    for(KService::List::Iterator it = offerList->begin();
		it != offerList->end();
		++it) {
		KSharedPtr<KService> *ptr = new KSharedPtr<KService>(*it);
		KService * currentOffer = ptr->data();

		VALUE obj = getPointerObject(currentOffer);
		if(obj == Qnil) {
		    smokeruby_object  * o = ALLOC(smokeruby_object);
		    o->smoke = m->smoke();
		    o->classId = m->smoke()->idClass("KService").index;
		    o->ptr = currentOffer;
		    o->allocated = false;
		    obj = set_obj_info("KDE::Service", o);
		}
		rb_ary_push(av, obj);
            }

	    *(m->var()) = av;		
	    
		if(m->cleanup())
		delete offerList;
		}
		break;
	default:
		m->unsupported();
		break;
	}
}

DEF_LIST_MARSHALLER( KActionList, QList<KAction*>, KAction )
DEF_LIST_MARSHALLER( KActionCollectionList, QList<KActionCollection*>, KActionCollection )
DEF_LIST_MARSHALLER( KJobList, QList<KJob*>, KJob )
DEF_LIST_MARSHALLER( KMainWindowList, QList<KMainWindow*>, KMainWindow )
DEF_LIST_MARSHALLER( KMultiTabBarButtonList, QList<KMultiTabBarButton*>, KMultiTabBarButton )
DEF_LIST_MARSHALLER( KMultiTabBarTabList, QList<KMultiTabBarTab*>, KMultiTabBarTab )
DEF_LIST_MARSHALLER( KNSEntryList, QList<KNS::Entry*>, KNS::Entry )
DEF_LIST_MARSHALLER( KPartsPartList, QList<KParts::Part*>, KParts::Part )
DEF_LIST_MARSHALLER( KPartsPluginList, QList<KParts::Plugin*>, KParts::Plugin )
DEF_LIST_MARSHALLER( KPartsReadOnlyPartList, QList<KParts::ReadOnlyPart*>, KParts::ReadOnlyPart )
DEF_LIST_MARSHALLER( KToolBarList, QList<KToolBar*>, KToolBar )
DEF_LIST_MARSHALLER( KXMLGUIClientList, QList<KXMLGUIClient*>, KXMLGUIClient )
DEF_LIST_MARSHALLER( KFileItemList, QList<KFileItem*>, KFileItem )


DEF_VALUELIST_MARSHALLER( KAboutPersonList, QList<KAboutPerson>, KAboutPerson )
DEF_VALUELIST_MARSHALLER( QColorList, QList<QColor>, QColor )
DEF_VALUELIST_MARSHALLER( ChoicesList, QList<KCoreConfigSkeleton::ItemEnum::Choice>, KCoreConfigSkeleton::ItemEnum::Choice )
DEF_VALUELIST_MARSHALLER( KDataToolInfoList, QList<KDataToolInfo>, KDataToolInfo )
DEF_VALUELIST_MARSHALLER( KIOCopyInfoList, QList<KIO::CopyInfo>, KIO::CopyInfo )
DEF_VALUELIST_MARSHALLER( KPartsPluginPluginInfoList, QList<KParts::Plugin::PluginInfo>, KParts::Plugin::PluginInfo )
DEF_VALUELIST_MARSHALLER( KUserList, QList<KUser>, KUser )
DEF_VALUELIST_MARSHALLER( KUserGroupList, QList<KUserGroup>, KUserGroup )
DEF_VALUELIST_MARSHALLER( KUrlList, QList<KUrl>, KUrl )
DEF_VALUELIST_MARSHALLER( KPluginInfoList, QList<KPluginInfo>, KPluginInfo )

/*
template <class Qt::Key, class Value, class ItemMapIterator, const char *KeySTR, const char *ValueSTR >
void marshall_Map(Marshall *m) {
    switch(m->action()) {
      case Marshall::FromVALUE:
	{
	    VALUE hash = *(m->var());
	    if (TYPE(hash) != T_HASH) {
		m->item().s_voidp = 0;
		break;
	    }
		
		QMap<Qt::Key,Value> * map = new QMap<Qt::Key,Value>;
		
		// Convert the ruby hash to an array of key/value arrays
		VALUE temp = rb_funcall(hash, rb_intern("to_a"), 0);

		for (long i = 0; i < RARRAY(temp)->len; i++) {
			VALUE key = rb_ary_entry(rb_ary_entry(temp, i), 0);
			VALUE value = rb_ary_entry(rb_ary_entry(temp, i), 1);
			
			smokeruby_object *o = value_obj_info(key);
			if( !o || !o->ptr)
                   continue;
			void * key_ptr = o->ptr;
			key_ptr = o->smoke->cast(key_ptr, o->classId, o->smoke->idClass(KeySTR));
			
			o = value_obj_info(value);
			if( !o || !o->ptr)
                   continue;
			void * val_ptr = o->ptr;
			val_ptr = o->smoke->cast(val_ptr, o->classId, o->smoke->idClass(ValueSTR));
			
			(*map)[(Qt::Key)*(Qt::Key*)key_ptr] = (Value)*(Value*)val_ptr;
		}
	    
		m->item().s_voidp = map;
		m->next();
		
	    if(m->cleanup())
		delete map;
	}
	break;
      case Marshall::ToVALUE:
	{
	    QMap<Qt::Key,Value> *map = (QMap<Qt::Key,Value>*)m->item().s_voidp;
	    if(!map) {
		*(m->var()) = Qnil;
		break;
	    }
		
	    VALUE hv = rb_hash_new();
	    
		int key_ix = m->smoke()->idClass(KeySTR);
	    const char * key_className = m->smoke()->binding->className(key_ix);
		
		int val_ix = m->smoke()->idClass(ValueSTR);
	    const char * val_className = m->smoke()->binding->className(val_ix);
			
		ItemMapIterator it;
		for (it = map->begin(); it != map->end(); ++it) {
			void *key_p = new Key(it.key());
			VALUE key_obj = getPointerObject(key_p);
			smokeruby_object  * o;
			
			if (key_obj == Qnil) {
				o = ALLOC(smokeruby_object);
				o->classId = m->smoke()->idClass(KeySTR);
				o->smoke = m->smoke();
				o->ptr = key_p;
				o->allocated = true;
				key_obj = set_obj_info(key_className, o);
			}
			
			void *val_p = new Value(it.data());
			VALUE value_obj = getPointerObject(val_p);
				
			if (value_obj == Qnil) {
				o = ALLOC(smokeruby_object);
				o->classId = m->smoke()->idClass(ValueSTR);
				o->smoke = m->smoke();
				o->ptr = val_p;
				o->allocated = true;
				value_obj = set_obj_info(val_className, o);
			}
			
			rb_hash_aset(hv, key_obj, value_obj);
        }
		
		*(m->var()) = hv;
		m->next();
		
	    if(m->cleanup())
		delete map;
	}
	break;
      default:
	m->unsupported();
	break;
    }
}

#define DEF_MAP_MARSHALLER(MapIdent,Qt::Key,Value) namespace { char KeyIdent##STR[] = #Qt::Key; char ValueIdent##STR[] = #Value; }  \
        Marshall::HandlerFn marshall_##MapIdent = marshall_Map<Qt::Key, Value,QMap<Qt::Key,Value>::Iterator,KeyIdent##STR, ValueIdent##STR>;

DEF_MAP_MARSHALLER( QMapKEntryKeyKEntry, KEntryKey, KEntry )

*/

TypeHandler KDE_handlers[] = {
    { "KSharedConfig::Ptr", marshall_KSharedConfigPtr },
    { "KSharedConfig::Ptr&", marshall_KSharedConfigPtr },
    { "KSharedConfigPtr", marshall_KSharedConfigPtr },
    { "KSharedConfigPtr&", marshall_KSharedConfigPtr },
    { "KService::Ptr", marshall_KServicePtr },
    { "KService::List", marshall_KServiceList },

    { "QList<KAboutPerson>", marshall_KAboutPersonList },
    { "QList<KAction*>", marshall_KActionList },
    { "QList<KActionCollection*>&", marshall_KActionCollectionList },
    { "QList<QColor>", marshall_QColorList },
    { "QList<QColor>&", marshall_QColorList },
    { "QList<KCoreConfigSkeleton::ItemEnum::Choice>", marshall_ChoicesList },
    { "QList<KCoreConfigSkeleton::ItemEnum::Choice>&", marshall_ChoicesList },
    { "QList<KDataToolInfo>", marshall_KDataToolInfoList },
    { "QList<KDataToolInfo>&", marshall_KDataToolInfoList },
    { "QList<KIO::CopyInfo>&", marshall_KIOCopyInfoList },
    { "QList<KJob*>&", marshall_KJobList },
    { "QList<KMainWindow*>&", marshall_KMainWindowList },
    { "QList<KMultiTabBarButton*>", marshall_KMultiTabBarButtonList },
    { "QList<KMultiTabBarTab*>", marshall_KMultiTabBarTabList },
    { "QList<KParts::Part*>", marshall_KPartsPartList },
    { "QList<KParts::Plugin*>", marshall_KPartsPluginList },
    { "QList<KParts::Plugin::PluginInfo>", marshall_KPartsPluginPluginInfoList },
    { "QList<KParts::Plugin::PluginInfo>&", marshall_KPartsPluginPluginInfoList },
    { "QList<KParts::ReadOnlyPart*>", marshall_KPartsReadOnlyPartList },
    { "QList<KPluginInfo>", marshall_KPluginInfoList },
    { "QList<KPluginInfo>&", marshall_KPluginInfoList },
    { "KPluginInfo::List", marshall_KPluginInfoList },
    { "KPluginInfo::List&", marshall_KPluginInfoList },
    { "QList<KToolBar*>", marshall_KToolBarList },
    { "QList<KUser>", marshall_KUserList },
    { "QList<KUser>&", marshall_KUserList },
    { "QList<KUserGroup>", marshall_KUserGroupList },
    { "QList<KXMLGUIClient*>&", marshall_KXMLGUIClientList },
    { "KFileItemList", marshall_KFileItemList },
    { "KFileItemList&", marshall_KFileItemList },
    { "KFileItemList*", marshall_KFileItemList },
    { "KNS::Entry::List", marshall_KNSEntryList },
    { "KUrl::List", marshall_KUrlList },
    { "KUrl::List&", marshall_KUrlList },
    { "KUrlList", marshall_KUrlList },
    { "KUrlList&", marshall_KUrlList },
    { 0, 0 }
};
