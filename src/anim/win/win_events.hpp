/* Copyright 2024 Fedor Borodulin

 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* "win_events.hpp" - window events and state file */

#ifndef __win_events_hpp__
#define __win_events_hpp__

#include <def.h>

#include "win_def.h"

#include <bitset>
#include <variant>
#include <optional>

/* Animation namespace // windows related namespace */
namespace anim::win
{
  /* Keyboard keys remapping to a custom define */
  enum class key : uint8_t
  {
    e_MouseMin         = 0x01u,
    eLButton           = 0x01u, // VK_LBUTTON define in Windows.h
    eRButton           = 0x02u, // VK_RBUTTON define in Windows.h
    eCancel            = 0x03u, // VK_CANCEL define in Windows.h
    eMButton           = 0x04u, // VK_MBUTTON define in Windows.h
    eXButton1          = 0x05u, // VK_XBUTTON1 define in Windows.h
    eXButton2          = 0x06u, // VK_XBUTTON2 define in Windows.h
    e_MouseMax         = 0x06u,
    /* 0x07 : reserved */
    eBack              = 0x08u, // VK_BACK define in Windows.h
    eTab               = 0x09u, // VK_TAB define in Windows.h
    /* 0x0A - 0x0B : reserved */
    eClear             = 0x0Cu, // VK_CLEAR define in Windows.h
    eEnter             = 0x0Du, // Equal to '\r', VK_RETURN define in Windows.h
    /* 0x0E - 0x0F : unassigned */
    eShift             = 0x10u, // VK_SHIFT define in Windows.h
    eCtrl              = 0x11u, // VK_CONTROL define in Windows.h
    eAlt               = 0x12u, // VK_MENU define in Windows.h
    ePause             = 0x13u, // VK_PAUSE define in Windows.h
    eCaps              = 0x14u, // VK_CAPITAL define in Windows.h
    /* 0x15 - 0x1A : Asia languages - not needed */
    eEsc               = 0x1Bu, // VK_ESCAPE define in Windows.h
    eConvert           = 0x1Cu, // VK_CONVERT define in Windows.h
    eNonConvert        = 0x1Du, // VK_NONCONVERT define in Windows.h
    eAccept            = 0x1Eu, // VK_ACCEPT define in Windows.h
    eModeChange        = 0x1Fu, // VK_MODECHANGE define in Windows.h
    eSpace             = 0x20u, // Equal to ' ', VK_SPACE define in Windows.h
    ePageUp            = 0x21u, // VK_PRIOR define in Windows.h
    ePageDown          = 0x22u, // VK_NEXT define in Windows.h
    eEnd               = 0x23u, // VK_END define in Windows.h
    eHome              = 0x24u, // VK_HOME define in Windows.h
    eArrowLeft         = 0x25u, // VK_LEFT define in Windows.h
    eArrowUp           = 0x26u, // VK_UP define in Windows.h
    eArrowRight        = 0x27u, // VK_RIGHT define in Windows.h
    eArrowDown         = 0x28u, // VK_DOWN define in Windows.h
    eSelect            = 0x29u, // VK_SELECT define in Windows.h
    ePrint             = 0x2Au, // VK_PRINT define in Windows.h
    eExec              = 0x2Bu, // VK_EXECUTE define in Windows.h
    ePrintScreen       = 0x2Cu, // VK_SNAPSHOT define in Windows.h
    eInsert            = 0x2Du, // VK_INSERT define in Windows.h
    eDel               = 0x2Eu, // VK_DELETE define in Windows.h
    eHelp              = 0x2Fu, // VK_HELP define in Windows.h
    e0                 = 0x30u, // Equal to '0'
    e1                 = 0x31u, // Equal to '1'
    e2                 = 0x32u, // Equal to '2'
    e3                 = 0x33u, // Equal to '3'
    e4                 = 0x34u, // Equal to '4'
    e5                 = 0x35u, // Equal to '5'
    e6                 = 0x36u, // Equal to '6'
    e7                 = 0x37u, // Equal to '7'
    e8                 = 0x38u, // Equal to '8'
    e9                 = 0x39u, // Equal to '9'
    /* 0x3A - 0x40 : unassigned */
    eA                 = 0x41u, // Equal to 'A'
    eB                 = 0x42u, // Equal to 'B'
    eC                 = 0x43u, // Equal to 'C'
    eD                 = 0x44u, // Equal to 'D'
    eE                 = 0x45u, // Equal to 'E'
    eF                 = 0x46u, // Equal to 'F'
    eG                 = 0x47u, // Equal to 'G'
    eH                 = 0x48u, // Equal to 'H'
    eI                 = 0x49u, // Equal to 'I'
    eJ                 = 0x4au, // Equal to 'J'
    eK                 = 0x4bu, // Equal to 'K'
    eL                 = 0x4cu, // Equal to 'L'
    eM                 = 0x4du, // Equal to 'M'
    eN                 = 0x4eu, // Equal to 'N'
    eO                 = 0x4fu, // Equal to 'O'
    eP                 = 0x50u, // Equal to 'P'
    eQ                 = 0x51u, // Equal to 'Q'
    eR                 = 0x52u, // Equal to 'R'
    eS                 = 0x53u, // Equal to 'S'
    eT                 = 0x54u, // Equal to 'T'
    eU                 = 0x55u, // Equal to 'U'
    eV                 = 0x56u, // Equal to 'V'
    eW                 = 0x57u, // Equal to 'W'
    eX                 = 0x58u, // Equal to 'X'
    eY                 = 0x59u, // Equal to 'Y'
    eZ                 = 0x5au, // Equal to 'Z'
    eLWin              = 0x5Bu, // VK_LWIN define in Windows.h
    eRWin              = 0x5Cu, // VK_RWIN define in Windows.h
    eApps              = 0x5Du, // VK_APPS define in Windows.h
    /* 0x5E : reserved */
    eSleep             = 0x5Fu, // VK_SLEEP define in Windows.h
    eNumPad0           = 0x60u, // VK_NUMPAD0 define in Windows.h
    eNumPad1           = 0x61u, // VK_NUMPAD1 define in Windows.h
    eNumPad2           = 0x62u, // VK_NUMPAD2 define in Windows.h
    eNumPad3           = 0x63u, // VK_NUMPAD3 define in Windows.h
    eNumPad4           = 0x64u, // VK_NUMPAD4 define in Windows.h
    eNumPad5           = 0x65u, // VK_NUMPAD5 define in Windows.h
    eNumPad6           = 0x66u, // VK_NUMPAD6 define in Windows.h
    eNumPad7           = 0x67u, // VK_NUMPAD7 define in Windows.h
    eNumPad8           = 0x68u, // VK_NUMPAD8 define in Windows.h
    eNumPad9           = 0x69u, // VK_NUMPAD9 define in Windows.h
    eMul               = 0x6Au, // VK_MULTIPLY define in Windows.h
    eAdd               = 0x6Bu, // VK_ADD define in Windows.h
    eSeparator         = 0x6Cu, // VK_SEPARATOR define in Windows.h
    eSub               = 0x6Du, // VK_SUBTRACT define in Windows.h
    eDecimal           = 0x6Eu, // VK_DECIMAL define in Windows.h
    eDiv               = 0x6Fu, // VK_DIVIDE define in Windows.h
    eF1                = 0x70u, // VK_F1 define in Windows.h
    eF2                = 0x71u, // VK_F2 define in Windows.h
    eF3                = 0x72u, // VK_F3 define in Windows.h
    eF4                = 0x73u, // VK_F4 define in Windows.h
    eF5                = 0x74u, // VK_F5 define in Windows.h
    eF6                = 0x75u, // VK_F6 define in Windows.h
    eF7                = 0x76u, // VK_F7 define in Windows.h
    eF8                = 0x77u, // VK_F8 define in Windows.h
    eF9                = 0x78u, // VK_F9 define in Windows.h
    eF10               = 0x79u, // VK_F10 define in Windows.h
    eF11               = 0x7Au, // VK_F11 define in Windows.h
    eF12               = 0x7Bu, // VK_F12 define in Windows.h
    /* 0x7C - 0x87 : F* from 13 to 24 - not needed
     * 0x88 - 0x8F : UI navigation - reserved */
    eNumLock           = 0x90u, // VK_NUMLOCK define in Windows.h
    eScrollLock        = 0x91u, // VK_SCROLL define in Windows.h
    /* 0x91 : NEC PC-9800 kbd definitions - not needed
     * 0x92 - 0x96 : Fujitsu/OASYS kbd definitions - not needed
     * 0x97 - 0x9F : unassigned */
    eLShift            = 0xA0u, // VK_LSHIFT define in Windows.h
    eRShift            = 0xA1u, // VK_RSHIFT define in Windows.h
    eLCtrl             = 0xA2u, // VK_LCONTROL define in Windows.h
    eRCtrl             = 0xA3u, // VK_RCONTROL define in Windows.h
    eLAlt              = 0xA4u, // VK_LMENU define in Windows.h
    eRAlt              = 0xA5u, // VK_RMENU define in Windows.h
    eBrowserBack       = 0xA6u, // VK_BROWSER_BACK define in Windows.h
    eBrowserForward    = 0xA7u, // VK_BROWSER_FORWARD define in Windows.h
    eBrowserRefresh    = 0xA8u, // VK_BROWSER_REFRESH define in Windows.h
    eBrowserStop       = 0xA9u, // VK_BROWSER_STOP define in Windows.h
    eBrowserSearch     = 0xAAu, // VK_BROWSER_SEARCH define in Windows.h
    eBrowserFavorites  = 0xABu, // VK_BROWSER_FAVORITES define in Windows.h
    eBrowserHome       = 0xACu, // VK_BROWSER_HOME define in Windows.h
    eVolumeMute        = 0xADu, // VK_VOLUME_MUTE define in Windows.h
    eVolumeDown        = 0xAEu, // VK_VOLUME_DOWN define in Windows.h
    eVolumeUp          = 0xAFu, // VK_VOLUME_UP define in Windows.h
    eMediaNextTrack    = 0xB0u, // VK_MEDIA_NEXT_TRACK define in Windows.h
    eMediaPrevTrack    = 0xB1u, // VK_MEDIA_PREV_TRACK define in Windows.h
    eMediaStop         = 0xB2u, // VK_MEDIA_STOP define in Windows.h
    eMediaPlayPause    = 0xB3u, // VK_MEDIA_PLAY_PAUSE define in Windows.h
    eLaunchMail        = 0xB4u, // VK_LAUNCH_MAIL define in Windows.h
    eLaunchMediaSelect = 0xB5u, // VK_LAUNCH_MEDIA_SELECT define in Windows.h
    eLaunchApp1        = 0xB6u, // VK_LAUNCH_APP1 define in Windows.h
    eLaunchApp2        = 0xB7u, // VK_LAUNCH_APP2 define in Windows.h

    _LastValue // Amount of values
  }; /* end of 'key' enumerable */

  /* Auxilary definitions for compile-time keys conversion */
  namespace key_convertion_help
  {
    /* Default declaration */
    template<char Char>
      struct key_converter
      {
        /* Conversion function */
        static consteval key Convert( void )
          requires(
            (Char >= '0' && Char <= '9') ||
            (Char >= 'A' && Char <= 'Z') ||
            Char == '\r' ||
            Char == ' '
          )
        {
          return static_cast<key>(Char);
        } /* End of 'Convert' function */

        /* Conversion function */
        static consteval key Convert( void )
          requires(
            Char >= 'a' && Char <= 'z'
          )
        {
          return static_cast<key>(Char - 'a' + 'A');
        } /* End of 'Convert' function */
      }; /* end of 'key_converter' strucuture */
  } /* end of 'key_convertion_help' namespace */

  /* Key from character code definition */
  template<char Char>
    constexpr key char_key {key_convertion_help::key_converter<Char>::Convert()};

  using keys_mask = std::bitset<(size_t)key::_LastValue>;

  /* Window events namespace */
  namespace events
  {
    /* Resize event */
    struct resize
    {
      uint32_t NewWidth {0}, NewHeight {0}; // New size (zero indicates collapsing)
      bool IsLast {true};                   // Indicates that this resize event is the final size
    }; /* end of 'resize' structure */
  
    /* Button state change event */
    struct keyboard
    {
      key Key {0};        // Button index
      bool State {false}; // New state
    }; /* end of 'keyboard' structure */
  
    /* Mouse moving event */
    struct mouse
    {
      float MouseDX {0.f}, MouseDY {0.f};     // Mouse move value
      float MouseNewX {0.f}, MouseNewY {0.f}; // Mouse new position
      float MouseDZ {0.f};                    // Mouse wheel
    }; /* end of 'mouse' structure */

    /* Window closing request event */
    struct close
    {
      bool Forced {false}; // Indicates that window has closed due to external reasons
    }; /* end of 'close' structure */
  
    using any = std::variant<
        std::nullopt_t,
        resize,
        keyboard,
        mouse,
        close
      >;
  } /* end of 'events' namespace */

  /* Window state data storage */
  class window_state
  {
    friend class window;

  public:
    /* State data */
    uint32_t Width {0}, Height {0};   // Window size
    keys_mask Keys {};                // Pressed keys mask
    float MouseX {0.f}, MouseY {0.f}; // Mouse window-relative coordinates 

    /* Empty constructor */
    window_state( void )
    {
    } /* End of constructor */

    /* State updating from event function
     * ARGUMENTS:
     *   - Event:
     *       const events::resize &Event;
     * RETURNS: None.
     */
    void Update( const events::resize &Event )
    {
      Width = Event.NewWidth;
      Height = Event.NewHeight;
    } /* End of 'Update' function */

    /* State updating from event function
     * ARGUMENTS:
     *   - Event:
     *       const events::keyboard &Event;
     * RETURNS: None.
     */
    void Update( const events::keyboard &Event )
    {
      keys_mask UpdateMask {1};
      UpdateMask <<= (uint8_t)Event.Key;

      if (Event.State)
        Keys |= UpdateMask;
      else
        Keys &= UpdateMask.flip();
    } /* End of 'Update' function */

    /* State updating from event function
     * ARGUMENTS:
     *   - Event:
     *       const events::mouse &Event;
     * RETURNS: None.
     */
    void Update( const events::mouse &Event )
    {
      MouseX = Event.MouseNewX;
      MouseY = Event.MouseNewY;
    } /* End of 'Update' function */
  }; /* end of 'window_state' class */

  /* Events queue class */
  class events_queue
  {
  private:
    /* Basically just fully locking all resources and do not care */
    std::atomic_size_t QueuedEventsCount {0}; // Count of currently queued events.
    std::mutex ResourcesSync {};              // Single syncronization primitive
    std::deque<events::any> QueueStorage {};  // deque fullfills all our needs here
    events::resize *LastResize {nullptr};     // Storing last resize to update corresponding flag

  public:
    /* Empty constructor */
    events_queue( void )
    {
    } /* End of constructor */

    /* Event appending function
     * ARGUMENTS:
     *   - Event:
     *       events::any &&Event;
     * RETURNS: None.
     */
    void Push( events::any &&Event )
    {
      std::visit(
        [this]( auto &Event )
        {
          using event_type = std::remove_cvref_t<decltype(Event)>;

          /* Quit if nullopt */
          if constexpr (std::same_as<event_type, std::nullopt_t>)
            return;

          /* Lock */
          std::lock_guard Lock {ResourcesSync};

          /* Update flag and pointer for resize event */
          if constexpr (std::same_as<event_type, events::resize>)
          {
            /* Ensure flag is correct */
            Event.IsLast = true;

            if (LastResize != nullptr)
              LastResize->IsLast = false;

            LastResize = &Event;
          }

          /* Add to queue */
          QueueStorage.emplace_back(std::forward<events::any>(Event));

          /* Update counter */
          if (QueuedEventsCount.fetch_add(1) == 0)
            QueuedEventsCount.notify_one();
        },
        Event
      );
    } /* End of 'Push' function */

    /* Event retrieving function
     * ARGUMENTS:
     *   - Event storage:
     *       events::any &Event;
     * RETURNS:
     *   (bool) Event existence flag.
     */
    bool Pop( events::any &Event )
    {
      /* Non-blocking check */
      if (QueuedEventsCount.load() == 0)
      {
        Event.emplace<std::nullopt_t>(std::nullopt);
        return false;
      }

      /* Update counter */
      QueuedEventsCount.fetch_sub(1, std::memory_order::relaxed);

      std::lock_guard Lock {ResourcesSync};

      /* Additional check (it's cheap now, just to be sure) */
      if (QueueStorage.empty())
      {
        Event.emplace<std::nullopt_t>(std::nullopt);
        return false;
      }

      Event.swap(QueueStorage.front());
      QueueStorage.pop_front();

      return true;
    } /* End of 'Pop' function */

    /* Event retrieving function. Waits.
     * ARGUMENTS:
     *   - Event storage:
     *       events::any &Event;
     * RETURNS:
     *   (bool) Event existence flag.
     */
    bool PopWait( events::any &Event )
    {
      /* Waiting */
      QueuedEventsCount.wait(0);

      /* Update counter */
      QueuedEventsCount.fetch_sub(1, std::memory_order::relaxed);

      std::lock_guard Lock {ResourcesSync};

      if (QueueStorage.empty())
      {
        Event.emplace<std::nullopt_t>(std::nullopt);
        return false;
      }

      Event.swap(QueueStorage.front());
      QueueStorage.pop_front();

      return true;
    } /* End of 'Pop' function */
  }; /* end of 'events_queue' class */
} /* end of 'win' namespace */

#endif /* __win_events_hpp__ */

/* END OF 'win_events.hpp' FILE */
