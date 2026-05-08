/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*
*	This product contains software technology licensed from Id
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc.
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>

#include "hud.h"
#include "cl_util.h"

#include "music.h"

Music g_Music;

Music::Music() : m_Soundtracks(), m_NumSoundtracks(0)
{
}

void Music::LoadMapSoundtracks(const char* soundtrack_file_path)
{
    // Open the file.
    FILE* fp = std::fopen(soundtrack_file_path, "r");

    if (fp)
    {
        // Reset values.
        m_NumSoundtracks = 0;
        std::memset(m_Soundtracks, 0, ARRAYSIZE(m_Soundtracks));

        char line[256] = {};

        while (!std::feof(fp) && m_NumSoundtracks < ARRAYSIZE(m_Soundtracks))
        {
            std::memset(line, 0, ARRAYSIZE(line));
            std::fgets(line, ARRAYSIZE(line), fp);

            // Skip empty or blank lines.
            if (line[0] == '\0' || line[0] == '\n' || std::isspace(line[0]))
                continue;

            // Skip lines that start with comments.
            if (line[0] == '/' || line[1] == '/')
                continue;

            soundtrack_t* psndtrack = &m_Soundtracks[m_NumSoundtracks];
            int ret = std::sscanf(line, " \"%[^\"]\" \"%[^\"]\" \"%d\" ", psndtrack->mapname, psndtrack->musicpath, &psndtrack->repeat);

            if (ret == 3)
            {
                m_NumSoundtracks++;
            }
            else
            {
                gEngfuncs.Con_Printf("Error occured while reading soundtrack: %s\n", line);
                // Reset data for next time.
                std::memset(psndtrack, 0, sizeof(*psndtrack));
            }
        }

        // Close the file.
        std::fclose(fp);
        fp = nullptr;
    }
    else
    {
        gEngfuncs.Con_Printf("Couldn't find %s!\n", soundtrack_file_path);
    }
}

soundtrack_t* Music::GetSountrackByMapName(const char* mapname)
{
    // Do not search if no soundtrack is available.
    if (m_NumSoundtracks == 0)
        return NULL;

    soundtrack_t* psndtrack = NULL;
    for (int i = 0; i < m_NumSoundtracks; ++i)
    {
        psndtrack = &m_Soundtracks[i];
        if (std::strcmp(psndtrack->mapname, mapname) == 0)
            return psndtrack;
    }

    return NULL;
}

BOOL Music::PlayMapMusic(const char* mapname)
{
    soundtrack_t* psndtrack = GetSountrackByMapName(mapname);
    if (psndtrack)
    {
        // Stop any MP3 soundtrack playing.
        EngineClientCmd("mp3 stop\n");

        // The original Poke646 release ships its tracks under sound/<path>,
        // but the reimplementation expects them under media/<path>. Probe both
        // locations; if neither has the track, refuse to play anything rather
        // than letting the engine fall back to some random file.
        const char* gamedir = gEngfuncs.pfnGetGameDirectory();
        const char* base_dir = nullptr;
        char probe[256] = {};

        std::snprintf(probe, ARRAYSIZE(probe), "%s/sound/%s", gamedir, psndtrack->musicpath);
        if (FILE* fp = std::fopen(probe, "rb"))
        {
            std::fclose(fp);
            base_dir = "sound";
        }
        else
        {
            std::snprintf(probe, ARRAYSIZE(probe), "%s/media/%s", gamedir, psndtrack->musicpath);
            if (FILE* fp2 = std::fopen(probe, "rb"))
            {
                std::fclose(fp2);
                base_dir = "media";
            }
        }

        if (!base_dir)
        {
            gEngfuncs.Con_Printf("Poke646 MP3-Player: track not found in sound/%s or media/%s; nothing will play.\n",
                                 psndtrack->musicpath, psndtrack->musicpath);
            return FALSE;
        }

        char command[128] = {};
        std::snprintf(command, ARRAYSIZE(command), "mp3 %s %s/%s\n",
                      psndtrack->repeat ? "loop" : "play",
                      base_dir, psndtrack->musicpath);

        // Play MP3 soundtrack.
        EngineClientCmd(command);

        gEngfuncs.Con_Printf("Poke646 MP3-Player is now playing: %s/%s\n",
                             base_dir, psndtrack->musicpath);

        return TRUE;
    }

    return FALSE;
}
