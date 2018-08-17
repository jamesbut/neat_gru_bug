#-------------------------------------------------
#
# Project created by QtCreator 2013-01-30T15:12:40
#
#-------------------------------------------------

QT       -= core gui

TARGET = aiTools
TEMPLATE = lib
CONFIG += staticlib
CONFIG += c++14

QMAKE_CXXFLAGS += -Wall -Wextra -Wconversion -std=c++14
QMAKE_CXXFLAGS_WARN_ON += -Wall -Wextra -Wconversion

INCLUDEPATH += ..

SOURCES += \
    Time/WallClock.cpp \
    Time/Time.cpp \
    Time/StopWatch.cpp \
    Util/Helper.cpp \
    Classification/ConfusionMatrix.cpp

contains(CONFIG, test) {
SOURCES += \
	Test/Test.cpp
HEADERS += \
	Test/Test.h
TEMPLATE = app
CONFIG -= staticlib
CONFIG += console
}

HEADERS += \
    Math/Geometry.h \
    Time/WallClock.h \
    Time/Time.h \
    Time/StopWatch.h \
    Util/Helper.h \
    Util/PlatformDetection.h \
    Math/Vector3.h \
    Math/Vector2.h \
    Util/RestrictedValue.h \
    Util/Histogram.h \
    Util/Range.h \
    Util/Trie.h \
    Util/Functors.h \
    Util/MetaprogrammingHelpers.h \
    Math/Statistics.h \
    Math/Distance.h \
    Serialization/Serialization.h \
    Serialization.h \
    Serialization/IntrinsicDatatypes.h \
    Serialization/Endianness.h \
    Serialization/STLDataTypes.h \
    Math/Vector.h \
    Math/MovingAverage.h \
    Algorithm/AStar.h \
    Algorithm/Dijkstra.h \
    Math/Angle.h \
    Math/FixedPrecision.h \
    Math/LinearTransformation.h \
    Math/Numeric.h \
    Util/RestoreStreamState.h \
    Threading/TaskQueue.h \
    Threading/ParallelForEach.h \
    Threading/RAIIThread.h \
    Math/SparseVector.h \
    Classification/ConfusionMatrix.h \
    Util/RandomizedForEach.h \
    Util/ForEachPair.h \
	Math/LosslessConversion.h \
    Util/IterateMultiple.h \
    Math/Quaternion.h \
    Util/Grid.h
    
unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/local/lib
    }
    INSTALLS += target
}
