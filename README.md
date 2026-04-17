# N503 Audio Library

---

## シンプルな設計

```cpp
#include <N503/Core/Window.hpp>
#include <N503/Audio/Source.hpp>
#include <format>

int main()
{
    N503::Core::Window coreWindow;
    N503::Audio::Source audioSource{ "Assets/Sample.wav" };

    coreWindow.OnEvent([&audioSource](N503::Core::Events::KeyInput& data)
    {
        if (data.CodePoint == 'a')
        {
            audioSource.Play();
        }
        else if (data.CodePoint == 's')
        {
            audioSource.Stop();
        }
        else if (data.CodePoint == 'd')
        {
            audioSource.Pause();
        }
        else if (data.CodePoint == 'f')
        {
            audioSource.Resume();
        }
        
        return false;
    });

    coreWindow.OnEvent([&coreWindow](N503::Core::Events::MouseMoved& data)
    {
        auto message = std::format("Mouse Moved: x={}, y={}\n", data.Location.X, data.Location.Y);
        coreWindow.SetTitle(message);
        return false;
    });

    coreWindow.WaitForQuit();
    return 0;
}
```

## 依存関係

- Windows11 / MediaFoundation
- N503.CppWin32.Abi
- N503.CppWin32.Memory


## ビルドシステム

Premake5

