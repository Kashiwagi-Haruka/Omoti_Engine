#pragma once
#include "Mesh/Primitive/Primitive.h"
#include "Object/Characters/Base/Attribute.h"
#include <cstdint>
#include <memory>
class Camera;

/// <summary>
/// 属性付着オブジェクト
/// </summary>
class Adhesion {

	// 前の属性板ポリ
	std::unique_ptr<Primitive> preAttributePlane_;
	// 現在の属性板ポリ
	std::unique_ptr<Primitive> AttributePlane_;
	// 属性付着反応の板ポリ
	std::unique_ptr<Primitive> AttributeReactionPlane_;

	// 属性付着反応の板ポリの変換情報
	uint32_t attributeBitMask_ = 0;
	// 今の属性
	Attribute currentAttribute_ = Attribute::None;

	// 属正反応の先についてた属性
	Attribute lastReactionPreviousAttribute_ = Attribute::None;
	// 属性付着反応の後側についてた属性
	Attribute lastReactionAppliedAttribute_ = Attribute::None;

	// 比較表示の有効性
	bool isComparisonDisplayActive_ = false;

	// 比較表示のタイマー
	float comparisonDisplayTimer_ = 0.0f;
	// ベーストランスフォーム
	Transform baseTransform_{};

	// ベーストランスフォームが設定されているかどうか
	bool hasBaseTransform_ = false;

	// カメラ
	Camera* camera_ = nullptr;

	/// <summary>
	/// 属性に対応するテクスチャインデックス
	/// </summary>
	/// <param name="attribute"> 属性 </param>
	/// <returns></returns>
	uint32_t ResolveTextureIndex(Attribute attribute) const;
	/// <summary>
	/// 属性付着反応に対応するテクスチャインデックス
	/// </summary>
	/// <param name="appliedAttribute"> 適用された属性 </param>
	/// <returns></returns>
	uint32_t ResolveReactionTextureIndex(Attribute appliedAttribute) const;

	/// <summary>
	/// 属性付着反応に対応するフレームインデックス
	/// </summary>
	/// <param name="appliedAttribute"> 適用された属性 </param>
	/// <param name="previousAttribute"> 前の属性 </param>
	/// <returns></returns>
	int ResolveReactionFrameIndex(Attribute appliedAttribute, Attribute previousAttribute) const;

	/// <summary>
	/// 属性テクスチャを更新する
	/// </summary>
	void RefreshAttributeTexture();

	/// <summary>
	/// 属性付着反応のテクスチャを更新する
	/// </summary>
	/// <param name="appliedAttribute"> 適用された属性 </param>
	/// <param name="previousAttribute"> 前の属性 </param>
	void RefreshReactionTexture(Attribute appliedAttribute, Attribute previousAttribute);

	/// <summary>
	/// 比較表示の変換を更新する
	/// </summary>
	void RefreshComparisonTransform();

	/// <summary>
	/// ビルボード変換を適用する
	/// </summary>
	/// <param name="primitive"> プリミティブ </param>
	/// <param name="transform"> 変換 </param>
	void ApplyBillboardTransform(Primitive* primitive, const Transform& transform) const;

public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Adhesion();
	/// <summary>
	/// デストラクタ
	/// </summary>
	~Adhesion();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();
	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// トランスフォームを設定する
	/// </summary>
	/// <param name="transform"> トランスフォーム </param>
	void SetTransform(const Transform& transform);

	/// <summary>
	/// カメラを設定する
	/// </summary>
	/// <param name="camera"> カメラ </param>
	void SetCamera(Camera* camera);

	/// <summary>
	/// 属性を追加する
	/// </summary>
	/// <param name="attribute"> 属性 </param>
	/// <returns></returns>
	bool AddAttribute(Attribute attribute);

	/// <summary>
	/// 最後の属性付着反応の前の属性を取得する
	/// </summary>
	/// <returns></returns>
	Attribute GetLastReactionPreviousAttribute() const { return lastReactionPreviousAttribute_; }
	/// <summary>
	/// 最後の属性付着反応の適用された属性を取得する
	/// </summary>
	/// <returns></returns>
	Attribute GetLastReactionAppliedAttribute() const { return lastReactionAppliedAttribute_; }
	/// <summary>
	/// 描画する
	/// </summary>
	void Draw();
};