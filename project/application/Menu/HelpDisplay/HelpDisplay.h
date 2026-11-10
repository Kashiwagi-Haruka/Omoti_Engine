#pragma once

#include "Sprite.h"
#include "Text/Text.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

class HelpDisplay {
public:
	HelpDisplay() = default;
	~HelpDisplay() = default;

	void Initialize();
	void Update();
	void Draw();
	void Finalize();

	void Open();
	void Close();

	bool IsOpen() const { return isOpen_; }

private:
	struct Rect {
		float x;
		float y;
		float width;
		float height;

		bool Contains(const Vector2& point) const { return point.x >= x && point.x < x + width && point.y >= y && point.y < y + height; }
	};

	struct HelpItem {
		std::u32string title;
		std::u32string description;
		std::string imagePath;
		uint32_t imageHandle = 0;

		std::unique_ptr<Sprite> background;
		std::unique_ptr<Text> titleText;
	};

	struct HelpTab {
		std::u32string title;

		std::unique_ptr<Sprite> background;
		std::unique_ptr<Text> titleText;

		std::vector<HelpItem> items;
	};

	static constexpr float kScreenWidth = 1280.0f;
	static constexpr float kScreenHeight = 720.0f;

	static constexpr float kRowHeight = 50.0f;
	static constexpr float kRowPitch = 66.0f;
	static constexpr float kScrollSpeed = 44.0f;
	static constexpr float kTextTopPadding = 32.0f;

	const Rect backRect_{20.0f, 20.0f, 90.0f, 48.0f};
	const Rect tabArea_{104.0f, 80.0f, 480.0f, 48.0f};
	const Rect listRect_{104.0f, 164.0f, 480.0f, 510.0f};

	const Rect titleRect_{640.0f, 38.0f, 576.0f, 70.0f};
	const Rect imageRect_{640.0f, 140.0f, 576.0f, 290.0f};
	const Rect detailRect_{640.0f, 460.0f, 576.0f, 214.0f};

	std::vector<HelpTab> tabs_;

	std::unique_ptr<Sprite> background_;
	std::unique_ptr<Sprite> backButton_;
	std::unique_ptr<Sprite> detailImage_;

	std::unique_ptr<Text> backText_;
	std::unique_ptr<Text> detailTitle_;
	std::unique_ptr<Text> detailText_;

	uint32_t whiteHandle_ = 0;
	uint32_t fontHandle_ = 0;

	std::size_t selectedTab_ = 0;
	std::size_t selectedItem_ = 0;

	float listScroll_ = 0.0f;
	float detailScroll_ = 0.0f;
	float detailContentHeight_ = 0.0f;

	bool initialized_ = false;
	bool isOpen_ = false;
	bool hasSelection_ = false;

	void LoadData();
	void SelectTab(std::size_t index);
	void SelectItem(std::size_t index);
	void UpdateLayout();

	Rect GetTabRect(std::size_t index) const;
	Rect GetItemRect(std::size_t index) const;

	float GetMaxListScroll() const;
	float GetMaxDetailScroll() const;

	std::unique_ptr<Sprite> MakeSprite(uint32_t textureHandle, const Rect& rect, const Vector4& color);

	std::unique_ptr<Text> MakeText(const std::u32string& text, float width, const Vector4& color);

	static std::u32string ToU32(const std::string& utf8);

	static void PlaceSprite(Sprite& sprite, const Rect& rect);

	static void PlaceText(Text& text, float x, float y, float width);

	static void SetClip(const Rect& rect);
	static void ResetClip();

	static void DrawSprite(Sprite& sprite);
};