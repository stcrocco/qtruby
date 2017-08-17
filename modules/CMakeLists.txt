project(QtRubyModules)

#macro_optional_add_bindings(QT_QTDECLARATIVE_FOUND "QtDeclarative" qtdeclarative)
#macro_optional_add_bindings(QT_QTWEBKIT_FOUND "QtWebKit" qtwebkit)
#macro_optional_add_bindings(QT_QTUITOOLS_FOUND "QtUiTools" qtuitools)
#macro_optional_add_bindings(QT_QTSCRIPT_FOUND "QtScript" qtscript)
#macro_optional_add_bindings(QT_QTTEST_FOUND "QtTest" qttest)

macro_optional_add_bindings(PHONON_FOUND "Phonon" phonon)

if(QSCINTILLA_FOUND AND QSCI_FOUND)
  set(BUILD_QSCINTILLA 1)
endif(QSCINTILLA_FOUND AND QSCI_FOUND)
macro_optional_add_bindings(BUILD_QSCINTILLA "QScintilla" qscintilla)

macro_optional_add_bindings(Qwt5_Qt4_FOUND "Qwt" qwt)

macro_display_bindings_log()
