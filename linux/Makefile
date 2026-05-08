# =============================================================================
# Makefile for the Linux build of the Poke646 mod (Half-Life)
# =============================================================================
# Builds two 32-bit shared libraries:
#   - hl.so      (game/server)
#   - client.so  (client)
#
# Usage:
#   make            # build both
#   make client     # client only
#   make server     # server only
#   make clean
# =============================================================================

CC      ?= gcc
CXX     ?= g++

# Half-Life engine on Linux is 32-bit, so the mod libraries must match.
ARCH_FLAGS := -m32

# Mod version. Keep this in sync with the project() VERSION in ../CMakeLists.txt.
MOD_VERSION := 2.0.2

# Source roots (relative to this Makefile's directory).
SRC      := ..
CL_DIR   := $(SRC)/cl_dll
DLL_DIR  := $(SRC)/dlls
COMMON   := $(SRC)/common
ENGINE   := $(SRC)/engine
PUBLIC   := $(SRC)/public
PM       := $(SRC)/pm_shared
GS       := $(SRC)/game_shared
UTILS    := $(SRC)/utils
EXTERNAL := $(SRC)/external
P646_CL  := $(CL_DIR)/poke646
P646_DLL := $(DLL_DIR)/poke646

CFG  ?= release
OBJDIR := $(CFG)/obj
OUTDIR := $(CFG)

ifeq ($(CFG),release)
  OPT_FLAGS := -O2
else
  OPT_FLAGS := -O0 -g
endif

# Common preprocessor / language flags that match the original Linux SDK build,
# trimmed to what modern GCC accepts and what Poke646 actually needs.
BASE_DEFS := \
  -DPOSIX -D_POSIX -DLINUX -D_LINUX -DGNUC -DHL1 \
  -DQUIVER -DVOXEL -DQUAKE2 -DCLIENT_WEAPONS \
  -Dstricmp=strcasecmp -Dstrcmpi=strcasecmp \
  -D_strnicmp=strncasecmp -Dstrnicmp=strncasecmp \
  -D_snprintf=snprintf -D_vsnprintf=vsnprintf \
  -D_alloca=alloca -DNO_MALLOC_OVERRIDE -DNDEBUG

WARN_FLAGS := -w -Werror=return-type
LANG_FLAGS := -fpermissive -fno-strict-aliasing -fno-exceptions -fvisibility=hidden \
              -fno-stack-protector

# Disable a few modern warnings that the legacy code base trips over.
COMPAT_FLAGS := -Wno-write-strings -Wno-narrowing -Wno-multichar -Wno-deprecated -Wno-format-security
# C++-only warning silencers (C compiler complains if these are passed to it).
CXX_COMPAT_FLAGS := -Wno-conversion-null -Wno-invalid-offsetof -Wno-class-memaccess

BASE_CFLAGS  := $(ARCH_FLAGS) $(OPT_FLAGS) $(BASE_DEFS) $(WARN_FLAGS) $(LANG_FLAGS) $(COMPAT_FLAGS)
# Pin language dialects: C23 makes `false`/`true` keywords, which collides with
# the qboolean enum in common/const.h. Pin to gnu11 for C, gnu++14 for C++ for
# a stable baseline that matches the original SDK's expectations.
C_STD   := -std=gnu11
CXX_STD := -std=gnu++14
SHLIB_FLAGS  := -shared
SHLIB_LDFLAGS := $(ARCH_FLAGS) -shared -Wl,-Bsymbolic

# -----------------------------------------------------------------------------
# Server library (hl.so)
# -----------------------------------------------------------------------------

SERVER_DEFS := -DVALVE_DLL

SERVER_INC := \
  -I$(DLL_DIR) -I$(P646_DLL) \
  -I$(ENGINE) -I$(COMMON) -I$(PM) -I$(GS) -I$(PUBLIC)

SERVER_CXXSRC := \
  $(DLL_DIR)/aflock.cpp \
  $(DLL_DIR)/agrunt.cpp \
  $(DLL_DIR)/airtank.cpp \
  $(DLL_DIR)/animating.cpp \
  $(DLL_DIR)/animation.cpp \
  $(DLL_DIR)/apache.cpp \
  $(DLL_DIR)/barnacle.cpp \
  $(DLL_DIR)/barney.cpp \
  $(DLL_DIR)/bigmomma.cpp \
  $(DLL_DIR)/bloater.cpp \
  $(DLL_DIR)/bmodels.cpp \
  $(DLL_DIR)/bullsquid.cpp \
  $(DLL_DIR)/buttons.cpp \
  $(DLL_DIR)/cbase.cpp \
  $(DLL_DIR)/client.cpp \
  $(DLL_DIR)/combat.cpp \
  $(DLL_DIR)/controller.cpp \
  $(DLL_DIR)/crossbow.cpp \
  $(DLL_DIR)/crowbar.cpp \
  $(DLL_DIR)/defaultai.cpp \
  $(DLL_DIR)/doors.cpp \
  $(DLL_DIR)/effects.cpp \
  $(DLL_DIR)/egon.cpp \
  $(DLL_DIR)/explode.cpp \
  $(DLL_DIR)/flyingmonster.cpp \
  $(DLL_DIR)/func_break.cpp \
  $(DLL_DIR)/func_tank.cpp \
  $(DLL_DIR)/game.cpp \
  $(DLL_DIR)/gamerules.cpp \
  $(DLL_DIR)/gargantua.cpp \
  $(DLL_DIR)/gauss.cpp \
  $(DLL_DIR)/genericmonster.cpp \
  $(DLL_DIR)/ggrenade.cpp \
  $(DLL_DIR)/globals.cpp \
  $(DLL_DIR)/gman.cpp \
  $(DLL_DIR)/hassassin.cpp \
  $(DLL_DIR)/headcrab.cpp \
  $(DLL_DIR)/healthkit.cpp \
  $(DLL_DIR)/hgrunt.cpp \
  $(DLL_DIR)/hornet.cpp \
  $(DLL_DIR)/houndeye.cpp \
  $(DLL_DIR)/h_ai.cpp \
  $(DLL_DIR)/h_battery.cpp \
  $(DLL_DIR)/h_cine.cpp \
  $(DLL_DIR)/h_cycler.cpp \
  $(DLL_DIR)/h_export.cpp \
  $(DLL_DIR)/ichthyosaur.cpp \
  $(DLL_DIR)/islave.cpp \
  $(DLL_DIR)/items.cpp \
  $(DLL_DIR)/leech.cpp \
  $(DLL_DIR)/lights.cpp \
  $(DLL_DIR)/maprules.cpp \
  $(DLL_DIR)/monstermaker.cpp \
  $(DLL_DIR)/monsters.cpp \
  $(DLL_DIR)/monsterstate.cpp \
  $(DLL_DIR)/mortar.cpp \
  $(DLL_DIR)/mp5.cpp \
  $(DLL_DIR)/multiplay_gamerules.cpp \
  $(DLL_DIR)/nihilanth.cpp \
  $(DLL_DIR)/nodes.cpp \
  $(DLL_DIR)/observer.cpp \
  $(DLL_DIR)/osprey.cpp \
  $(DLL_DIR)/pathcorner.cpp \
  $(DLL_DIR)/plane.cpp \
  $(DLL_DIR)/plats.cpp \
  $(DLL_DIR)/player.cpp \
  $(DLL_DIR)/python.cpp \
  $(DLL_DIR)/rat.cpp \
  $(DLL_DIR)/roach.cpp \
  $(DLL_DIR)/rpg.cpp \
  $(DLL_DIR)/satchel.cpp \
  $(DLL_DIR)/schedule.cpp \
  $(DLL_DIR)/scientist.cpp \
  $(DLL_DIR)/scripted.cpp \
  $(DLL_DIR)/shotgun.cpp \
  $(DLL_DIR)/singleplay_gamerules.cpp \
  $(DLL_DIR)/skill.cpp \
  $(DLL_DIR)/sound.cpp \
  $(DLL_DIR)/soundent.cpp \
  $(DLL_DIR)/spectator.cpp \
  $(DLL_DIR)/squadmonster.cpp \
  $(DLL_DIR)/squeakgrenade.cpp \
  $(DLL_DIR)/subs.cpp \
  $(DLL_DIR)/talkmonster.cpp \
  $(DLL_DIR)/teamplay_gamerules.cpp \
  $(DLL_DIR)/tempmonster.cpp \
  $(DLL_DIR)/tentacle.cpp \
  $(DLL_DIR)/triggers.cpp \
  $(DLL_DIR)/tripmine.cpp \
  $(DLL_DIR)/turret.cpp \
  $(DLL_DIR)/util.cpp \
  $(DLL_DIR)/weapons.cpp \
  $(DLL_DIR)/world.cpp \
  $(DLL_DIR)/wpn_shared/hl_wpn_glock.cpp \
  $(DLL_DIR)/xen.cpp \
  $(DLL_DIR)/zombie.cpp \
  $(GS)/voice_gamemgr.cpp \
  $(P646_DLL)/firetrail.cpp \
  $(P646_DLL)/genericmodel.cpp \
  $(P646_DLL)/nail.cpp \
  $(P646_DLL)/robocop.cpp \
  $(P646_DLL)/xenspit.cpp

SERVER_CSRC := \
  $(PM)/pm_debug.c \
  $(PM)/pm_math.c \
  $(PM)/pm_shared.c

SERVER_OBJS := \
  $(SERVER_CXXSRC:$(SRC)/%.cpp=$(OBJDIR)/server/%.o) \
  $(SERVER_CSRC:$(SRC)/%.c=$(OBJDIR)/server/%.o)

# -----------------------------------------------------------------------------
# Client library (client.so)
# -----------------------------------------------------------------------------

# Client gets vec3_origin and the vector helpers from the engine, so the
# stand-alone copies in pm_math.c must be omitted from the client build.
CLIENT_DEFS := -DCLIENT_DLL -DHL_DLL -DDISABLE_JUMP_ORIGIN -DDISABLE_VEC_ORIGIN

CLIENT_INC := \
  -I$(DLL_DIR) -I$(P646_DLL) \
  -I$(CL_DIR) -I$(P646_CL) \
  -I$(PUBLIC) -I$(COMMON) -I$(PM) -I$(ENGINE) \
  -I$(UTILS)/vgui/include -I$(GS) -I$(EXTERNAL)

CLIENT_CXXSRC := \
  $(CL_DIR)/ammo.cpp \
  $(CL_DIR)/ammohistory.cpp \
  $(CL_DIR)/ammo_secondary.cpp \
  $(CL_DIR)/battery.cpp \
  $(CL_DIR)/cdll_int.cpp \
  $(CL_DIR)/com_weapons.cpp \
  $(CL_DIR)/death.cpp \
  $(CL_DIR)/demo.cpp \
  $(CL_DIR)/entity.cpp \
  $(CL_DIR)/events.cpp \
  $(CL_DIR)/ev_common.cpp \
  $(CL_DIR)/ev_hldm.cpp \
  $(CL_DIR)/flashlight.cpp \
  $(CL_DIR)/GameStudioModelRenderer.cpp \
  $(CL_DIR)/geiger.cpp \
  $(CL_DIR)/health.cpp \
  $(CL_DIR)/hl/hl_baseentity.cpp \
  $(CL_DIR)/hl/hl_events.cpp \
  $(CL_DIR)/hl/hl_objects.cpp \
  $(CL_DIR)/hl/hl_weapons.cpp \
  $(CL_DIR)/hud.cpp \
  $(CL_DIR)/hud_bench.cpp \
  $(CL_DIR)/hud_benchtrace.cpp \
  $(CL_DIR)/hud_msg.cpp \
  $(CL_DIR)/hud_redraw.cpp \
  $(CL_DIR)/hud_servers.cpp \
  $(CL_DIR)/hud_spectator.cpp \
  $(CL_DIR)/hud_update.cpp \
  $(CL_DIR)/input.cpp \
  $(CL_DIR)/inputw32.cpp \
  $(CL_DIR)/interpolation.cpp \
  $(CL_DIR)/in_camera.cpp \
  $(CL_DIR)/menu.cpp \
  $(CL_DIR)/message.cpp \
  $(CL_DIR)/saytext.cpp \
  $(CL_DIR)/statusbar.cpp \
  $(CL_DIR)/status_icons.cpp \
  $(CL_DIR)/StudioModelRenderer.cpp \
  $(CL_DIR)/studio_util.cpp \
  $(CL_DIR)/text_message.cpp \
  $(CL_DIR)/train.cpp \
  $(CL_DIR)/tri.cpp \
  $(CL_DIR)/util.cpp \
  $(CL_DIR)/vgui_ClassMenu.cpp \
  $(CL_DIR)/vgui_ControlConfigPanel.cpp \
  $(CL_DIR)/vgui_CustomObjects.cpp \
  $(CL_DIR)/vgui_int.cpp \
  $(CL_DIR)/vgui_MOTDWindow.cpp \
  $(CL_DIR)/vgui_SchemeManager.cpp \
  $(CL_DIR)/vgui_ScorePanel.cpp \
  $(CL_DIR)/vgui_ServerBrowser.cpp \
  $(CL_DIR)/vgui_SpectatorPanel.cpp \
  $(CL_DIR)/vgui_TeamFortressViewport.cpp \
  $(CL_DIR)/vgui_teammenu.cpp \
  $(CL_DIR)/view.cpp \
  $(CL_DIR)/voice_status.cpp \
  $(COMMON)/parsemsg.cpp \
  $(DLL_DIR)/crossbow.cpp \
  $(DLL_DIR)/crowbar.cpp \
  $(DLL_DIR)/egon.cpp \
  $(DLL_DIR)/gauss.cpp \
  $(DLL_DIR)/mp5.cpp \
  $(DLL_DIR)/python.cpp \
  $(DLL_DIR)/rpg.cpp \
  $(DLL_DIR)/satchel.cpp \
  $(DLL_DIR)/shotgun.cpp \
  $(DLL_DIR)/squeakgrenade.cpp \
  $(DLL_DIR)/tripmine.cpp \
  $(DLL_DIR)/wpn_shared/hl_wpn_glock.cpp \
  $(GS)/vgui_checkbutton2.cpp \
  $(GS)/vgui_grid.cpp \
  $(GS)/vgui_helpers.cpp \
  $(GS)/vgui_listbox.cpp \
  $(GS)/vgui_loadtga.cpp \
  $(GS)/vgui_scrollbar2.cpp \
  $(GS)/vgui_slider2.cpp \
  $(GS)/voice_banmgr.cpp \
  $(PUBLIC)/interface.cpp \
  $(P646_CL)/music.cpp \
  $(P646_CL)/scope.cpp

CLIENT_CSRC := \
  $(PM)/pm_debug.c \
  $(PM)/pm_math.c \
  $(PM)/pm_shared.c

CLIENT_OBJS := \
  $(CLIENT_CXXSRC:$(SRC)/%.cpp=$(OBJDIR)/client/%.o) \
  $(CLIENT_CSRC:$(SRC)/%.c=$(OBJDIR)/client/%.o)

# Use the bundled 32-bit vgui.so / SDL2 from this directory at link time.
VGUI_LIB  := vgui.so
SDL2_LIB  := libSDL2-2.0.so.0

CLIENT_LDLIBS := -L. $(VGUI_LIB) $(SDL2_LIB) -ldl -lpthread -lm
SERVER_LDLIBS := -ldl -lpthread -lm

# -----------------------------------------------------------------------------
# Targets
# -----------------------------------------------------------------------------

.PHONY: all client server clean dirs mod_config
all: mod_config client server

client: mod_config $(OUTDIR)/client.so
server: mod_config $(OUTDIR)/hl.so

# Generate common/mod_config.h from the cmake template, the same way the
# Windows cmake build does it. Regenerate only when the template or version
# changes so we don't trigger needless rebuilds.
MOD_CONFIG_OUT := $(COMMON)/mod_config.h
MOD_CONFIG_IN  := $(SRC)/cmake/mod_config.h.in

mod_config: $(MOD_CONFIG_OUT)

$(MOD_CONFIG_OUT): $(MOD_CONFIG_IN) Makefile
	@echo "Generating $@ (version $(MOD_VERSION))"
	@sed 's/@PROJECT_VERSION@/$(MOD_VERSION)/g' $< > $@

$(OUTDIR)/hl.so: $(SERVER_OBJS) | $(OUTDIR)
	$(CXX) $(SHLIB_LDFLAGS) -o $@ $(SERVER_OBJS) $(SERVER_LDLIBS)

$(OUTDIR)/client.so: $(CLIENT_OBJS) | $(OUTDIR)
	$(CXX) $(SHLIB_LDFLAGS) -o $@ $(CLIENT_OBJS) $(CLIENT_LDLIBS)

$(OUTDIR):
	@mkdir -p $@

# Server compile rules
$(OBJDIR)/server/%.o: $(SRC)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXX_STD) $(BASE_CFLAGS) $(CXX_COMPAT_FLAGS) $(SERVER_DEFS) $(SERVER_INC) -c $< -o $@

$(OBJDIR)/server/%.o: $(SRC)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(C_STD) $(BASE_CFLAGS) $(SERVER_DEFS) $(SERVER_INC) -c $< -o $@

# Client compile rules
$(OBJDIR)/client/%.o: $(SRC)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXX_STD) $(BASE_CFLAGS) $(CXX_COMPAT_FLAGS) $(CLIENT_DEFS) $(CLIENT_INC) -c $< -o $@

$(OBJDIR)/client/%.o: $(SRC)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(C_STD) $(BASE_CFLAGS) $(CLIENT_DEFS) $(CLIENT_INC) -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(OUTDIR)/hl.so $(OUTDIR)/client.so
