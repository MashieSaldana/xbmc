set(SOURCES VideoLibraryCleaningJob.cpp
            VideoLibraryJob.cpp
            VideoLibraryMarkWatchedJob.cpp
            VideoLibraryProgressJob.cpp
            VideoLibraryRefreshingJob.cpp
            VideoLibraryRescanStreamDetailsJob.cpp
            VideoLibraryScanningJob.cpp
            VideoLibraryResetResumePointJob.cpp)

set(HEADERS VideoLibraryCleaningJob.h
            VideoLibraryJob.h
            VideoLibraryMarkWatchedJob.h
            VideoLibraryProgressJob.h
            VideoLibraryRefreshingJob.h
            VideoLibraryRescanStreamDetailsJob.h
            VideoLibraryScanningJob.h
            VideoLibraryResetResumePointJob.h)

core_add_library(video_jobs)
