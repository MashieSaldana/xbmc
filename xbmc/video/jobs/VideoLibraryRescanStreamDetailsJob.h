/*
 *  Copyright (C) 2026 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "video/jobs/VideoLibraryProgressJob.h"

class CGUIDialogProgressBarHandle;

/*!
 \brief Re-read video and audio stream details for every library item.

 Reads the media files only; no scraper, artwork or text metadata is touched.
*/
class CVideoLibraryRescanStreamDetailsJob : public CVideoLibraryProgressJob
{
public:
  CVideoLibraryRescanStreamDetailsJob();
  explicit CVideoLibraryRescanStreamDetailsJob(CGUIDialogProgressBarHandle* progressBar);
  ~CVideoLibraryRescanStreamDetailsJob() override;

  // specialization of CJob
  const char* GetType() const override { return "VideoLibraryRescanStreamDetailsJob"; }
  bool operator==(const CJob* job) const override;

protected:
  // implementation of CVideoLibraryJob
  bool Work(CVideoDatabase& db) override;
};