#pragma once

// 1. Project Headers
#include "../Frames/Buffer.hpp"
#include "Context.hpp"

// 2. Project Dependencies
#include <N503/Audio/Format.hpp>

// 3. WIL (Windows Implementation Library)
#include <wil/resource.h>

// 4. Third-party Libraries

// 5. Windows Headers
#include <Windows.h>
#include <xaudio2.h>

// 6. C++ Standard Libraries
#include <atomic>
#include <cstdint>

namespace N503::Audio::Device
{

    /// @brief IXAudio2SourceVoice をラップし、個別の音声再生を制御するクラス
    /// @details XAudio2 のボイスコールバック（IXAudio2VoiceCallback）を継承しており、
    ///          オーディオデータのサブミット、開始、停止、および再生完了通知の受信を担当します。
    class SourceVoice : public IXAudio2VoiceCallback
    {
    public:
        /// @brief コンストラクタ
        /// @param context オーディオコンテキストへのポインタ
        /// @param format 使用するオーディオフォーマット（サンプリングレート、チャンネル数など）
        SourceVoice(Context *context, const Audio::Format &format);

        /// @brief デストラクタ
        /// @note 内部で保持している XAudio2 のボイスリソースを明示的に破棄します。
        virtual ~SourceVoice() noexcept;

        /// @brief 音声の出力を開始します。
        /// @return 成功した場合は true
        auto Start() -> bool;

        /// @brief 音声の出力を一時停止します。
        /// @return 成功した場合は true
        auto Stop() -> bool;

        /// @brief 音声の出力を一時停止します。(同期版)
        /// @note この関数はバッファをXAudio2が消費した事を保証します
        /// @return 成功した場合は true
        auto StopAndWait() -> bool;

        /// @brief 未再生のオーディオバッファをすべて破棄（フラッシュ）します。
        /// @return 成功した場合は true
        auto Flush() -> bool;

        /// @brief オーディオバッファを再生キューに登録（サブミット）します。
        /// @param buffer 登録する波形データ
        /// @param pBufferContext コールバック関数（OnBufferEnd等）に渡される任意のポインタ
        /// @return サブミットに成功した場合は true
        auto Submit(const Frames::Buffer &buffer, void *pBufferContext) -> bool;

        /// @brief このボイスに関連付けられているフォーマット情報を取得します。
        /// @return Format 構造体への参照
        auto GetFormat() const noexcept -> const Audio::Format &;

        /// @brief 出力ボリュームを設定します。
        /// @param volume ボリューム値
        /// @return 成功した場合は true
        auto SetVolume(float volume) -> bool;

        /// @brief 現在のボリュームを取得します。
        /// @return ボリューム値
        auto GetVolume() const noexcept -> float;

        /// @brief キューに存在するバッファ数を取得します。
        /// @return キューに存在するバッファ数
        auto GetBuffersQueued() const noexcept -> std::uint32_t;

        // --- IXAudio2VoiceCallback インターフェースの実装 ---

        /// @brief 音声処理パスが開始される直前に呼ばれます（空実装）。
        void STDMETHODCALLTYPE OnVoiceProcessingPassStart(UINT32 BytesRequired) override
        {
        }

        /// @brief 音声処理パスが終了した直後に呼ばれます（空実装）。
        void STDMETHODCALLTYPE OnVoiceProcessingPassEnd() override
        {
        }

        /// @brief オーディオストリームの終わりに達したときに呼ばれます（空実装）。
        void STDMETHODCALLTYPE OnStreamEnd() override
        {
        }

        /// @brief 個別のバッファの再生が開始されたときに呼ばれます。
        void STDMETHODCALLTYPE OnBufferStart(void *pBufferContext) override;

        /// @brief 個別のバッファの再生が完了したときに呼ばれます。
        void STDMETHODCALLTYPE OnBufferEnd(void *pBufferContext) override;

        /// @brief ループ領域の最後に達したときに呼ばれます。
        void STDMETHODCALLTYPE OnLoopEnd(void *pBufferContext) override;

        /// @brief 音声処理中に重大なエラーが発生したときに呼ばれます（空実装）。
        void STDMETHODCALLTYPE OnVoiceError(void *pBufferContext, HRESULT Error) override
        {
        }

    private:
        /// @brief 実体となる XAudio2 ソースボイス
        IXAudio2SourceVoice *m_SourceVoice{nullptr};

        /// @brief このボイスのオーディオフォーマット
        N503::Audio::Format m_Format{};

        /// @brief 停止イベントを表す手動リセットイベントオブジェクト。初期状態は非シグナル状態です。
        wil::unique_event m_StoppedEvent{::CreateEventW(nullptr, TRUE, FALSE, nullptr)};

        /// @brief 保留中のバッファの数を追跡するアトミックなカウンター。スレッドセーフな操作のために 0
        /// で初期化されます。
        std::atomic<std::uint64_t> m_PendingBuffers{0};
    };

} // namespace N503::Audio::Device
