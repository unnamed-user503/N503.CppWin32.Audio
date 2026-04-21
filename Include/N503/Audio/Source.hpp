#pragma once

// 1. Project Headers

// 2. Project Dependencies
#include <N503/Abi/Api.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <memory>
#include <string_view>

namespace N503::Audio
{

    class N503_API Source final
    {
    public:
        explicit Source(std::string_view filepath);

        ~Source();

        Source(const Source &) = delete;

        auto operator=(const Source &) -> Source & = delete;

        Source(Source &&) = default;

        auto operator=(Source &&) -> Source & = default;

    public:
        auto Play() -> void;

        auto Stop() -> void;

        auto Pause() -> void;

        auto Resume() -> void;

    public:
        /// @brief 実装の詳細を隠蔽するための不透明な構造体。
        struct Entity;

#ifdef N503_DLL_EXPORTS
        /// @brief 内部の実装実体（Entity）への参照を取得します。
        /// @note このメソッドはライブラリ内部（DLL境界の内側）でのみ使用されます。
        /// @return Entity を管理する unique_ptr への参照。
        [[nodiscard]]
        auto GetEntity() -> std::unique_ptr<Entity> &
        {
            return m_Entity;
        }
#endif

    private:
#pragma warning(push)
#pragma warning(disable : 4251) // DLL境界を越える際の unique_ptr に関する警告を抑制
        /// @brief
        std::unique_ptr<Entity> m_Entity;
#pragma warning(pop)
    };

} // namespace N503::Audio
