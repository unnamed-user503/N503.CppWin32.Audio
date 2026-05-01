#pragma once

// 1. Project Headers
#include "Context.hpp"
#include "Descriptor.hpp"

// 2. Project Dependencies
#include <N503/Audio/Format.hpp>
#include <N503/Audio/Status.hpp>
#include <N503/Audio/Types.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers
#include <Windows.h>

// 6. C++ Standard Libraries
#include <chrono>
#include <format>
#include <iostream>
#include <memory>
#include <type_traits>
#include <utility>

namespace N503::Audio::Node
{

    template <typename... TNodes> class Path : public TNodes...
    {
    public:
        // 各ノードのコンストラクタに必要な引数を、それぞれの基底クラスへ転送
        // TArgs... は各ノードの初期化用オブジェクトなどを想定
        template <typename... TArgs> explicit Path(TArgs&&... args) : TNodes(std::forward<TArgs>(args))... // 各基底クラスを初期化
        {
            m_Context = std::make_unique<Node::Context>();
        }

        auto Process() -> bool
        {
            // 内部でエラーが発生しているので強制終了
            if (m_Context->Descriptor.Status == Audio::Status::Error)
            {
                ImmediateStop();
                return true;
            }

            // 各ノードのメイン処理(Update)を末端から実行する
            bool isFinished = Update<sizeof...(TNodes) - 1>(*m_Context);

            // 全てのノードが終了を許可(trueを返した)時始めて停止できる
            if (isFinished)
            {
                if (m_Context->Descriptor.Repeat)
                {
                    m_Context->Descriptor.Status = Audio::Status::Playing;

                    // clang-format off
                    ([&]<typename T>()
                    {
                        auto& node = static_cast<T&>(*this);
                        if constexpr (requires { node.OnRepeat(); })
                        {
                            node.OnRepeat();
                        }
                    }.template operator()<TNodes>(), ...);
                    // clang-format on

                    return false;
                }

                // このパスを停止させる
                ImmediateStop();
#ifdef _DEBUG
                ::OutputDebugStringA("[Audio] Node::Path: All nodes finished. Process terminal.\n");
#endif
                return true;
            }

            return false; // 誰か一人でも false (継続) を返している間は再生を維持
        }

        auto Play(const Node::Descriptor& descriptor, const Audio::Format& format) -> bool
        {
            if (m_Context->Descriptor.Status != Audio::Status::Stopped)
            {
                return false;
            }

            // Context のリセットとセットアップ (Statisticsは維持する)
            m_Context->Descriptor        = descriptor;
            m_Context->Descriptor.Status = Audio::Status::Loading;

            m_Context->Effect                = {};
            m_Context->Effect.Fade.Threshold = std::chrono::milliseconds(320);
            m_Context->Effect.Fade.Elapsed   = std::chrono::milliseconds(0);
            m_Context->Effect.Fade.Direction = std::chrono::milliseconds(-16);

            m_Context->Time     = {};
            m_Context->Position = {};

            m_Format = format;

            // clang-format off
            ([&]<typename T>()
            {
                auto& node = static_cast<T&>(*this);

                if constexpr (requires { node.OnPlay(format); })
                {
                    node.OnPlay(format);
                }
                if constexpr (requires { node.OnPlay(descriptor); })
                {
                    node.OnPlay(descriptor);
                }
                if constexpr (requires { node.OnPlay(); })
                {
                    node.OnPlay();
                }

            }.template operator()<TNodes>(), ...);
            // clang-format on

            return true;
        }

        auto Stop() -> bool
        {
            // 停止要求時にリピートフラグを下ろしておく事でスムーズに停止へ遷移できる
            m_Context->Descriptor.Repeat = false;

            if (m_Context->Descriptor.Status == Audio::Status::Paused)
            {
                m_Context->Descriptor.Status     = Audio::Status::Stopping;
                m_Context->Effect.Fade.Threshold = std::chrono::microseconds(320);
                m_Context->Effect.Fade.Elapsed   = std::chrono::microseconds(0);
                m_Context->Effect.Fade.Direction = std::chrono::microseconds(-16);
                return true;
            }
            else if (m_Context->Descriptor.Status != Audio::Status::Playing || m_Context->Descriptor.Status == Audio::Status::Stopping)
            {
                return false;
            }
            else
            {
                m_Context->Descriptor.Status = Audio::Status::Stopping;
            }

            // Audio::TypeがStaticの場合フェード処理は必用ないので直接Stopを行う
            if (m_Context->Descriptor.Type == Audio::Type::Static)
            {
                return ImmediateStop();
            }

            m_Context->Effect.Fade.Threshold = std::chrono::microseconds(640);
            m_Context->Effect.Fade.Elapsed   = std::chrono::microseconds(0);
            m_Context->Effect.Fade.Direction = std::chrono::microseconds(16);

            return true;
        }

        auto ImmediateStop() -> bool
        {
            // 停止要求時にリピートフラグを下ろしておく事でスムーズに停止へ遷移できる
            m_Context->Descriptor.Repeat = false;

            if (m_Context->Descriptor.Status == Audio::Status::Stopped)
            {
                return false;
            }

            // clang-format off
            ([&]<typename T>()
            {
                using TNode = TNodes;
                auto& node = static_cast<TNode&>(*this);

                if constexpr (requires { node.OnStop(); })
                {
                    node.OnStop();
                }

            }.template operator()<TNodes>(), ...);
            // clang-format on

            // Descriptorをクリアして、Process()が次回即座にtrueを返せるようにする
            m_Context->Descriptor        = {};
            m_Context->Descriptor.Status = Audio::Status::Stopped;
            return true;
        }

        auto Pause() -> bool
        {
            // 再生中以外（既に一時停止中や停止処理中）なら何もしない
            if (m_Context->Descriptor.Status != Audio::Status::Playing)
            {
                return false;
            }

            m_Context->Descriptor.Status = Audio::Status::Paused;
            return true;
        }

        auto Resume() -> bool
        {
            // 一時停止中以外なら何もしない
            if (m_Context->Descriptor.Status != Audio::Status::Paused)
            {
                return false;
            }

            m_Context->Descriptor.Status = Audio::Status::Playing;
            return true;
        }

        auto Fade(std::chrono::microseconds threshold, std::chrono::microseconds direction) -> bool
        {
            if (m_Context->Descriptor.Status != Audio::Status::Playing)
            {
                return false;
            }

            m_Context->Effect.Fade.Threshold = threshold;
            m_Context->Effect.Fade.Direction = direction;

            // clang-format off
            ([&]<typename T>()
            {
                auto& node = static_cast<T&>(*this);

                if constexpr (requires { node.OnFade(threshold, direction); })
                {
                    node.OnFade(threshold, direction);
                }

            }.template operator()<TNodes>(), ...);
            // clang-format on

            return true;
        }

    private:
        template <std::size_t Index> auto Update(Context& context) -> bool
        {
            using TNode = std::tuple_element_t<Index, std::tuple<TNodes...>>;

            // 先に現在のノードを動かす
            bool currentFinished = static_cast<TNode*>(this)->Update(context);
#ifdef _DEBUG
            //::OutputDebugStringA(std::format("Node[{}]={}, ", Index, currentFinished ? "o" : "x").data());
            std::cout << std::format("Node[{}]={}, ", Index, currentFinished ? "o" : "x");
#endif
            if constexpr (Index > 0)
            {
                // 次のノードの結果を先に取得し、短絡評価を防ぐ
                bool forwardFinished = Update<Index - 1>(context);
                return currentFinished && forwardFinished;
            }
#ifdef _DEBUG
            //::OutputDebugStringA("\n");
            std::cout << std::endl;
#endif
            return currentFinished;
        }

    public:
        auto GetStatus() const -> Audio::Status
        {
            return m_Context->Descriptor.Status;
        }

        auto GetAssetHandle() const -> const Audio::Resource::Handle
        {
            // Stopping中ででも停止したかのように振舞うとフェードアウト中に次の曲が再生できる
            if (GetStatus() == Audio::Status::Stopping || GetStatus() == Audio::Status::Stopped)
            {
                return {};
            }

            return m_Context->Descriptor.Handle;
        }

    private:
        std::unique_ptr<Context> m_Context;

        Audio::Format m_Format{};
    };

    template <typename... TArgs> Path(std::unique_ptr<Context>, TArgs&&...) -> Path<std::decay_t<TArgs>...>;

} // namespace N503::Audio::Node
