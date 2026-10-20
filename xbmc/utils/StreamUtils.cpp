/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "StreamUtils.h"

extern "C"
{
#include <libavcodec/avcodec.h>
}

std::string StreamUtils::GetCodecName(int codecId, int profile)
{
  if (codecId == AV_CODEC_ID_DTS)
  {
    switch (profile)
    {
      case FF_PROFILE_DTS_HD_MA:
        return "dtshd_ma";
      case FF_PROFILE_DTS_HD_MA_X:
        return "dtshd_ma_x";
      case FF_PROFILE_DTS_HD_MA_X_IMAX:
        return "dtshd_ma_x_imax";
      case FF_PROFILE_DTS_HD_HRA:
        return "dtshd_hra";
      default:
        return "dca";
    }
  }

  if (codecId == AV_CODEC_ID_EAC3 && profile == FF_PROFILE_EAC3_DDP_ATMOS)
    return "eac3_ddp_atmos";

  if (codecId == AV_CODEC_ID_TRUEHD && profile == FF_PROFILE_TRUEHD_ATMOS)
    return "truehd_atmos";

  const AVCodec* codec = avcodec_find_decoder(static_cast<AVCodecID>(codecId));
  if (codec)
    return avcodec_get_name(codec->id);

  return {};
}

int StreamUtils::GetCodecPriority(const std::string &codec)
{
  /*
   * Technically flac, truehd, and dtshd_ma are equivalently good as they're all lossless. However,
   * ffmpeg can't decode dtshd_ma losslessy yet.
   */
  if (codec == "truehd_atmos") // Dolby TrueHD with Atmos
    return 7;
  if (codec == "dtshd_ma_x" || codec == "dtshd_ma_x_imax") // DTS-HD MA with DTS:X
    return 6;
  if (codec == "eac3_ddp_atmos") // Dolby Digital Plus with Atmos
    return 4;
  if (codec == "flac") // Lossless FLAC
    return 7;
  if (codec == "truehd") // Dolby TrueHD
    return 6;
  if (codec == "dtshd_ma") // DTS-HD Master Audio (previously known as DTS++)
    return 5;
  if (codec == "dtshd_hra") // DTS-HD High Resolution Audio
    return 4;
  if (codec == "eac3") // Dolby Digital Plus
    return 3;
  if (codec == "dca") // DTS
    return 2;
  if (codec == "ac3") // Dolby Digital
    return 1;
  return 0;
}
