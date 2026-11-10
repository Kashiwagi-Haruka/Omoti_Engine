#ifndef NOMINMAX
#define NOMINMAX
#endif

#include "HelpDisplay.h"

#include "DirectXCommon.h"
#include "Input.h"
#include "SpriteCommon.h"
#include "TextureManager.h"

#include "Engine/Logger/Logger.h"
#include "Text/FreetypeManager/FreeTypeManager.h"

#include <Windows.h>
#include <nlohmann/json.hpp>

#include <algorithm>
#include <cmath>
#include <fstream>
#include <limits>
#include <stdexcept>
#include <utility>

namespace {
constexpr const char* kDataPath = "Resources/JSON/HelpDisplay.json";

constexpr const char* kWhitePath = "Resources/2d/white2x2.png";

constexpr const char* kFontPath = "Resources/Font/irohakakuC-Bold.ttf";

const Vector4 kWhite{1.0f, 1.0f, 1.0f, 1.0f};
const Vector4 kBlack{0.0f, 0.0f, 0.0f, 1.0f};
const Vector4 kBackground{0.08f, 0.08f, 0.10f, 1.0f};
const Vector4 kButton{0.25f, 0.25f, 0.30f, 1.0f};

const Vector4 kSelectedTab{0.72f, 0.84f, 1.0f, 1.0f};
const Vector4 kSelectedItem{0.86f, 0.92f, 1.0f, 1.0f};
} // namespace

void HelpDisplay::Initialize() {
	if (initialized_) {
		return;
	}

	auto* textures = TextureManager::GetInstance();

	whiteHandle_ = textures->GetTextureIndexByfilePath(kWhitePath);

	fontHandle_ = FreeTypeManager::CreateFace(kFontPath, 0);
	FreeTypeManager::SetPixelSizes(fontHandle_, 24, 24);

	background_ = MakeSprite(whiteHandle_, {0.0f, 0.0f, kScreenWidth, kScreenHeight}, kBackground);

	backButton_ = MakeSprite(whiteHandle_, backRect_, kButton);

	detailImage_ = MakeSprite(whiteHandle_, imageRect_, kWhite);

	backText_ = MakeText(U"戻る", backRect_.width - 16.0f, kWhite);

	detailTitle_ = MakeText(U"", titleRect_.width, kWhite);

	detailText_ = MakeText(U"", detailRect_.width - 16.0f, kWhite);

	LoadData();

	initialized_ = true;
	isOpen_ = false;

	SelectTab(0);
}

void HelpDisplay::LoadData() {
	// 読み込み途中の失敗で半端なデータを残さないよう、
	// 一時変数で構築してから反映する。
	std::vector<HelpTab> loadedTabs;

	try {
		std::ifstream file(kDataPath, std::ios::binary);

		if (!file) {
			throw std::runtime_error("Cannot open HelpDisplay.json");
		}

		nlohmann::json root;
		file >> root;

		const auto& tabData = root.at("tabs");

		if (!tabData.is_array()) {
			throw std::runtime_error("'tabs' must be an array");
		}

		for (const auto& sourceTab : tabData) {
			HelpTab tab;

			tab.title = ToU32(sourceTab.at("title").get<std::string>());

			tab.background = MakeSprite(whiteHandle_, {0.0f, 0.0f, 1.0f, 1.0f}, kWhite);

			tab.titleText = MakeText(tab.title, tabArea_.width, kBlack);

			const auto& itemData = sourceTab.at("items");

			if (!itemData.is_array()) {
				throw std::runtime_error("'items' must be an array");
			}

			for (const auto& sourceItem : itemData) {
				HelpItem item;

				item.title = ToU32(sourceItem.at("title").get<std::string>());

				item.description = ToU32(sourceItem.at("description").get<std::string>());

				item.imagePath = sourceItem.value("image", std::string(kWhitePath));

				if (item.imagePath.empty()) {
					item.imagePath = kWhitePath;
				}

				item.imageHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath(item.imagePath);

				item.background = MakeSprite(whiteHandle_, {0.0f, 0.0f, listRect_.width, kRowHeight}, kWhite);

				item.titleText = MakeText(item.title, listRect_.width - 24.0f, kBlack);

				tab.items.push_back(std::move(item));
			}

			loadedTabs.push_back(std::move(tab));
		}

		tabs_ = std::move(loadedTabs);
	} catch (const std::exception& error) {
		tabs_.clear();

		Logger::Log(std::string("HelpDisplay: ") + error.what() + "\n");
	}
}

void HelpDisplay::Open() {
	if (!initialized_) {
		Initialize();
	}

	isOpen_ = true;

	Input::GetInstance()->SetIsCursorStability(false);
	Input::GetInstance()->SetIsCursorVisible(true);

	UpdateLayout();
}

void HelpDisplay::Close() {
	isOpen_ = false;

	// カーソルの表示・固定を戻す処理は、
	// 戻り先のメニューまたはゲーム側で行う。
}

void HelpDisplay::Finalize() {
	Close();

	tabs_.clear();

	background_.reset();
	backButton_.reset();
	detailImage_.reset();

	backText_.reset();
	detailTitle_.reset();
	detailText_.reset();

	initialized_ = false;
	hasSelection_ = false;

	selectedTab_ = 0;
	selectedItem_ = 0;

	listScroll_ = 0.0f;
	detailScroll_ = 0.0f;
	detailContentHeight_ = 0.0f;

	// フォント・テクスチャ自体は、
	// エンジンの各Managerが管理する。
}

void HelpDisplay::Update() {
	if (!initialized_ || !isOpen_) {
		return;
	}

	auto* input = Input::GetInstance();

	const Vector2 mouse{input->GetMouseX(), input->GetMouseY()};

	const bool clicked = input->TriggerMouseButton(Input::MouseButton::kLeft);

	if (input->TriggerKey(DIK_ESCAPE) || (clicked && backRect_.Contains(mouse))) {
		Close();
		return;
	}

	if (clicked) {
		for (std::size_t i = 0; i < tabs_.size(); ++i) {
			if (GetTabRect(i).Contains(mouse)) {
				SelectTab(i);
				break;
			}
		}
	}

	// Inputのホイール値は生のlZなので、
	// 標準の1ノッチである120で割る。
	const float wheel = input->GetMouseWheelDelta() / static_cast<float>(WHEEL_DELTA);

	if (wheel != 0.0f) {
		if (listRect_.Contains(mouse)) {
			listScroll_ = std::clamp(listScroll_ - wheel * kScrollSpeed, 0.0f, GetMaxListScroll());
		} else if (detailRect_.Contains(mouse)) {
			detailScroll_ = std::clamp(detailScroll_ - wheel * kScrollSpeed, 0.0f, GetMaxDetailScroll());
		}
	}

	if (clicked && listRect_.Contains(mouse) && selectedTab_ < tabs_.size()) {
		const auto& items = tabs_[selectedTab_].items;

		for (std::size_t i = 0; i < items.size(); ++i) {
			if (GetItemRect(i).Contains(mouse)) {
				SelectItem(i);
				break;
			}
		}
	}

	UpdateLayout();
}

void HelpDisplay::SelectTab(std::size_t index) {
	hasSelection_ = false;

	selectedItem_ = 0;
	listScroll_ = 0.0f;
	detailScroll_ = 0.0f;
	detailContentHeight_ = 0.0f;

	detailTitle_->SetString(U"");
	detailText_->SetString(U"");

	if (tabs_.empty()) {
		selectedTab_ = 0;

		detailTitle_->SetString(U"ヒントがありません");

		UpdateLayout();
		return;
	}

	selectedTab_ = std::min(index, tabs_.size() - 1);

	if (tabs_[selectedTab_].items.empty()) {
		detailTitle_->SetString(U"このタブにはヒントがありません");

		UpdateLayout();
		return;
	}

	SelectItem(0);
}

void HelpDisplay::SelectItem(std::size_t index) {
	if (selectedTab_ >= tabs_.size()) {
		return;
	}

	auto& items = tabs_[selectedTab_].items;

	if (index >= items.size()) {
		return;
	}

	selectedItem_ = index;
	hasSelection_ = true;
	detailScroll_ = 0.0f;

	auto& item = items[selectedItem_];

	// 左右で同じtitleを使う。
	detailTitle_->SetString(item.title);
	detailText_->SetString(item.description);

	detailImage_->SetTextureHandle(item.imageHandle);

	// 実際のフォントの折り返し結果から、
	// 詳細のスクロール範囲を求める。
	const auto runs = FreeTypeManager::LayoutString(fontHandle_, item.description, {0.0f, 0.0f}, detailRect_.width - 16.0f);

	detailContentHeight_ = 0.0f;

	if (!runs.empty()) {
		float lastBaseline = runs.front().position.y;

		for (const auto& run : runs) {
			lastBaseline = std::max(lastBaseline, run.position.y);
		}

		// 上下の余白も含めて、最終行まで表示できる高さ。
		detailContentHeight_ = kTextTopPadding + lastBaseline + 32.0f;
	}

	UpdateLayout();
}

HelpDisplay::Rect HelpDisplay::GetTabRect(std::size_t index) const {
	if (tabs_.empty()) {
		return tabArea_;
	}

	constexpr float gap = 8.0f;

	const float slotWidth = tabArea_.width / static_cast<float>(tabs_.size());

	const float buttonWidth = std::max(1.0f, slotWidth - gap);

	return {tabArea_.x + slotWidth * static_cast<float>(index), tabArea_.y, buttonWidth, tabArea_.height};
}

HelpDisplay::Rect HelpDisplay::GetItemRect(std::size_t index) const { return {listRect_.x, listRect_.y + kRowPitch * static_cast<float>(index) - listScroll_, listRect_.width, kRowHeight}; }

float HelpDisplay::GetMaxListScroll() const {
	if (selectedTab_ >= tabs_.size()) {
		return 0.0f;
	}

	const auto count = tabs_[selectedTab_].items.size();

	if (count == 0) {
		return 0.0f;
	}

	const float contentHeight = static_cast<float>(count - 1) * kRowPitch + kRowHeight;

	return std::max(0.0f, contentHeight - listRect_.height);
}

float HelpDisplay::GetMaxDetailScroll() const { return std::max(0.0f, detailContentHeight_ - detailRect_.height); }

void HelpDisplay::UpdateLayout() {
	PlaceText(*backText_, backRect_.x + 12.0f, backRect_.y + kTextTopPadding, backRect_.width - 24.0f);

	for (std::size_t i = 0; i < tabs_.size(); ++i) {
		auto& tab = tabs_[i];
		const Rect rect = GetTabRect(i);

		PlaceSprite(*tab.background, rect);

		tab.background->SetColor(i == selectedTab_ ? kSelectedTab : kWhite);

		tab.background->Update();

		PlaceText(*tab.titleText, rect.x + 8.0f, rect.y + kTextTopPadding, std::max(1.0f, rect.width - 16.0f));
	}

	if (selectedTab_ < tabs_.size()) {
		auto& items = tabs_[selectedTab_].items;

		for (std::size_t i = 0; i < items.size(); ++i) {
			auto& item = items[i];
			const Rect rect = GetItemRect(i);

			PlaceSprite(*item.background, rect);

			item.background->SetColor(hasSelection_ && i == selectedItem_ ? kSelectedItem : kWhite);

			item.background->Update();

			PlaceText(*item.titleText, rect.x + 12.0f, rect.y + kTextTopPadding, rect.width - 24.0f);
		}
	}

	PlaceSprite(*detailImage_, imageRect_);

	PlaceText(*detailTitle_, titleRect_.x, titleRect_.y + kTextTopPadding, titleRect_.width);

	PlaceText(*detailText_, detailRect_.x + 8.0f, detailRect_.y + kTextTopPadding - detailScroll_, detailRect_.width - 16.0f);
}

void HelpDisplay::Draw() {
	if (!initialized_ || !isOpen_) {
		return;
	}

	ResetClip();

	DrawSprite(*background_);
	DrawSprite(*backButton_);

	SetClip(backRect_);
	backText_->Draw();

	for (std::size_t i = 0; i < tabs_.size(); ++i) {
		const Rect rect = GetTabRect(i);
		auto& tab = tabs_[i];

		SetClip(rect);

		DrawSprite(*tab.background);
		tab.titleText->Draw();
	}

	if (selectedTab_ < tabs_.size()) {
		auto& items = tabs_[selectedTab_].items;

		for (std::size_t i = 0; i < items.size(); ++i) {
			const Rect rect = GetItemRect(i);

			const float top = std::max(rect.y, listRect_.y);

			const float bottom = std::min(rect.y + rect.height, listRect_.y + listRect_.height);

			if (bottom <= top) {
				continue;
			}

			// 白い画像と見出しを同じ領域で切り抜く。
			SetClip({listRect_.x, top, listRect_.width, bottom - top});

			DrawSprite(*items[i].background);
			items[i].titleText->Draw();
		}
	}

	SetClip(titleRect_);
	detailTitle_->Draw();

	if (hasSelection_) {
		SetClip(imageRect_);
		DrawSprite(*detailImage_);

		SetClip(detailRect_);
		detailText_->Draw();
	}

	// 次の描画へクリッピングやフォント用PSOを残さない。
	ResetClip();

	SpriteCommon::GetInstance()->DrawCommon();
}

std::unique_ptr<Sprite> HelpDisplay::MakeSprite(uint32_t textureHandle, const Rect& rect, const Vector4& color) {
	auto sprite = std::make_unique<Sprite>();

	sprite->Initialize(textureHandle);
	sprite->SetColor(color);

	PlaceSprite(*sprite, rect);

	return sprite;
}

std::unique_ptr<Text> HelpDisplay::MakeText(const std::u32string& text, float width, const Vector4& color) {
	auto label = std::make_unique<Text>();

	label->Initialize(fontHandle_);
	label->SetAlign(TextAlign::Left);
	label->SetColor(color);
	label->SetString(text);

	PlaceText(*label, 0.0f, 0.0f, width);

	return label;
}

void HelpDisplay::PlaceSprite(Sprite& sprite, const Rect& rect) {
	// Spriteの標準アンカーは左上。
	sprite.SetPosition({rect.x, rect.y});
	sprite.SetScale({rect.width, rect.height});
	sprite.Update();
}

void HelpDisplay::PlaceText(Text& text, float x, float y, float width) {
	text.SetPosition({x, y});
	text.SetSize({width, kScreenHeight});

	// 現在のTextは位置やサイズを変えただけでは
	// 描画座標が更新されないため、再レイアウトする。
	text.UpdateLayout(false);
}

void HelpDisplay::DrawSprite(Sprite& sprite) {
	// Text::Drawがフォント用PSOへ切り替えるため、
	// Spriteを描く前にSprite用PSOへ戻す。
	SpriteCommon::GetInstance()->DrawCommon();
	sprite.Draw();
}

void HelpDisplay::SetClip(const Rect& rect) {
	const float left = std::clamp(rect.x, 0.0f, kScreenWidth);

	const float top = std::clamp(rect.y, 0.0f, kScreenHeight);

	const float right = std::clamp(rect.x + rect.width, left, kScreenWidth);

	const float bottom = std::clamp(rect.y + rect.height, top, kScreenHeight);

	const D3D12_RECT clip{static_cast<LONG>(std::floor(left)), static_cast<LONG>(std::floor(top)), static_cast<LONG>(std::ceil(right)), static_cast<LONG>(std::ceil(bottom))};

	SpriteCommon::GetInstance()->GetDxCommon()->GetCommandList()->RSSetScissorRects(1, &clip);
}

void HelpDisplay::ResetClip() { SetClip({0.0f, 0.0f, kScreenWidth, kScreenHeight}); }

std::u32string HelpDisplay::ToU32(const std::string& utf8) {
	if (utf8.empty()) {
		return {};
	}

	if (utf8.size() > static_cast<std::size_t>((std::numeric_limits<int>::max)())) {
		throw std::runtime_error("UTF-8 text is too long");
	}

	const int length = static_cast<int>(utf8.size());

	const int wideLength = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, utf8.data(), length, nullptr, 0);

	if (wideLength <= 0) {
		throw std::runtime_error("Invalid UTF-8 in HelpDisplay.json");
	}

	std::wstring wide(static_cast<std::size_t>(wideLength), L'\0');

	if (MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, utf8.data(), length, wide.data(), wideLength) != wideLength) {
		throw std::runtime_error("UTF-8 conversion failed");
	}

	std::u32string result;
	result.reserve(wide.size());

	for (std::size_t i = 0; i < wide.size(); ++i) {
		const char32_t high = static_cast<char32_t>(wide[i]);

		if (high >= 0xD800 && high <= 0xDBFF) {
			if (i + 1 >= wide.size()) {
				throw std::runtime_error("Invalid surrogate pair");
			}

			const char32_t low = static_cast<char32_t>(wide[++i]);

			if (low < 0xDC00 || low > 0xDFFF) {
				throw std::runtime_error("Invalid surrogate pair");
			}

			result.push_back(0x10000 + ((high - 0xD800) << 10) + (low - 0xDC00));
		} else if (high >= 0xDC00 && high <= 0xDFFF) {
			throw std::runtime_error("Invalid surrogate pair");
		} else if (high != U'\r') {
			// CRLFのCRは除去し、LFだけを改行に使う。
			result.push_back(high);
		}
	}

	return result;
}