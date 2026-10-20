/*
 *  Copyright (C) 2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "VideoLibraryRescanStreamDetailsJob.h"

#include "FileItem.h"
#include "cores/VideoPlayer/DVDFileInfo.h"
#include "dialogs/GUIDialogExtendedProgressBar.h"
#include "guilib/LocalizeStrings.h"
#include "utils/URIUtils.h"
#include "utils/log.h"
#include "video/VideoDatabase.h"

#include <cstring>

CVideoLibraryRescanStreamDetailsJob::CVideoLibraryRescanStreamDetailsJob()
  : CVideoLibraryProgressJob(nullptr)
{
}

CVideoLibraryRescanStreamDetailsJob::CVideoLibraryRescanStreamDetailsJob(
    CGUIDialogProgressBarHandle* progressBar)
  : CVideoLibraryProgressJob(progressBar)
{
}

CVideoLibraryRescanStreamDetailsJob::~CVideoLibraryRescanStreamDetailsJob() = default;

bool CVideoLibraryRescanStreamDetailsJob::operator==(const CJob* job) const
{
  return strcmp(job->GetType(), GetType()) == 0;
}

bool CVideoLibraryRescanStreamDetailsJob::Work(CVideoDatabase& db)
{
  std::vector<std::pair<int, std::string>> files;
  db.GetLibraryFilesForStreamRescan(files);

  if (files.empty())
  {
    MarkFinished();
    return true;
  }

  SetTitle(g_localizeStrings.Get(39202));
  SetProgress(0, static_cast<int>(files.size()));

  int done = 0;
  int updated = 0;
  int failed = 0;

  db.BeginTransaction();
  for (const auto& [idFile, filePath] : files)
  {
    if (IsCancelled())
      break;

    SetText(URIUtils::GetFileName(filePath));

    CFileItem item(filePath, false);
    if (CDVDFileInfo::GetFileStreamDetails(&item))
    {
      db.SetStreamDetailsForFileId(item.GetVideoInfoTag()->m_streamDetails, idFile);
      ++updated;
    }
    else
    {
      ++failed;
    }

    SetProgress(++done, static_cast<int>(files.size()));
  }
  db.CommitTransaction();

  CLog::Log(LOGINFO, "Rescan stream details: processed {}, updated {}, failed {}, cancelled {}",
            done, updated, failed, IsCancelled());

  MarkFinished();
  return true;
}