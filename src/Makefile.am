NULL =

layoutincludedir = $(pkgincludedir)/layout
layoutinclude_HEADERS = \
	LEFontInstance.h \
	LEGlyphFilter.h \
	LEGlyphStorage.h \
	LEInsertionList.h \
	LELanguages.h \
	LEScripts.h \
	LESwaps.h \
	LETypes.h \
	LayoutEngine.h \
	loengine.h \
	$(NULL)

lib_LTLIBRARIES = libicu-le-aat.la

libicu_le_aat_la_LDFLAGS =
libicu_le_aat_la_CPPFLAGS = \
	$(ICU_CFLAGS) \
	$(NULL)
libicu_le_aat_la_LIBADD = \
	$(NULL)

libicu_le_aat_la_SOURCES = \
	$(include_HEADERS) \
	ContextualGlyphInsertion.h \
	ContextualGlyphInsertionProc2.cpp \
	ContextualGlyphInsertionProc2.h \
	ContextualGlyphSubstProc.cpp \
	ContextualGlyphSubstProc.h \
	ContextualGlyphSubstProc2.cpp \
	ContextualGlyphSubstProc2.h \
	ContextualGlyphSubstitution.h \
	ContextualKerning.h \
	ContextualKerningProc.cpp \
	ContextualKerningProc.h \
	GXLayoutEngine.cpp \
	GXLayoutEngine.h \
	GXLayoutEngine2.cpp \
	GXLayoutEngine2.h \
	IndicRearrangement.cpp \
	IndicRearrangement.h \
	IndicRearrangementProcessor.cpp \
	IndicRearrangementProcessor.h \
	IndicRearrangementProcessor2.cpp \
	IndicRearrangementProcessor2.h \
	KernStateTables.h \
	KernTables.cpp \
	KernTables.h \
	LEFontInstance.cpp \
	LEGlyphStorage.cpp \
	LEInsertionList.cpp \
	LETableReference.h \
	LayoutEngine.cpp \
	LayoutTables.h \
	LigatureSubstProc.cpp \
	LigatureSubstProc.h \
	LigatureSubstProc2.cpp \
	LigatureSubstProc2.h \
	LigatureSubstitution.h \
	LookupTableProcessor.cpp \
	LookupTableProcessor.h \
	LookupTables.cpp \
	LookupTables.h \
	MorphStateTables.h \
	MorphTables.cpp \
	MorphTables.h \
	MorphTables2.cpp \
	NonContextualGlyphSubst.h \
	NonContextualGlyphSubstProc.cpp \
	NonContextualGlyphSubstProc.h \
	NonContextualGlyphSubstProc2.h \
	OpenTypeTables.h \
	OrderedListKerningPairs.h \
	OrderedListKerningPairsProc.cpp \
	OrderedListKerningPairsProc.h \
	ScriptAndLanguageTags.cpp \
	ScriptAndLanguageTags.h \
	StateTableProcessor.cpp \
	StateTableProcessor.h \
	StateTableProcessor2.cpp \
	StateTableProcessor2.h \
	StateTables.h \
	SubtableProcessor.cpp \
	SubtableProcessor.h \
	loengine.cpp
	$(NULL)

-include $(top_srcdir)/git.mk
