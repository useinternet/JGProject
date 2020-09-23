#pragma once
#include "Component.h"
#include <filesystem>



namespace JGUI
{
	class GUI_API DrawComponent : public Component
	{


	protected:
		virtual void Awake()   override;
		virtual void Start()   override;
		virtual void Tick()    override;
		virtual void Destroy() override;

	public:
		void SetColor(const JColor& color);
		void SetDrawPriority(uint64_t priority) {
			m_DrawPriority = priority;
		}
		void SetAutoPriority(bool is_auto) {
			m_IsAutoPriority = is_auto;
		}
		void SetInteractionActive(bool is_ia) {
			m_IsInteractionActive = is_ia;
		}

		const JColor& GetColor() const {
			return m_Color;
		}

	protected:
		virtual void ConvertVertexCoordinate(const JRect& localRect);
		virtual bool InteractionActive();
		virtual bool IsDirty();
		virtual void ClearDirty();
	protected:
		SceneObjectRef m_SceneObjectRef;

		std::vector<PaperVertex> m_V;
		std::vector<uint32_t>    m_I;
		SDObject m_Instance;

		JVector2 m_PrevPivot = { FLT_MAX, FLT_MAX };
		JVector2 m_PrevSize = { -1, -1 };

		bool     m_IsAutoPriority = true;
		bool     m_IsInteractionActive     = true;
		uint64_t m_DrawPriority = 0;



		size_t m_PrevVertexCount = 0;
		size_t m_PrevIndexCount = 0;

		JColor m_Color;

		//LayerComponent* m_Layer = nullptr;
	};


	class GUI_API ImageComponent : public DrawComponent
	{
	protected:
		virtual void Awake()   override;
	public:
		void SetImage(GE::Texture* t);
		void SetImage(const std::wstring& imagePath);
		GE::Texture* GetTexture() const;
	private:
		std::wstring   m_ImageSource;
	};


	class GUI_API ShapeComponent : public DrawComponent
	{
		using ConvertFunc = std::function<void(const JRect&, std::vector<PaperVertex>&, std::vector<uint32_t>&)>;
		using InteractionFunc = std::function<bool(const JRect&)>;
	protected:
		virtual void Awake()   override;
		virtual void ConvertVertexCoordinate(const JRect& localRect) override;
		virtual bool InteractionActive() override;
		virtual void ClearDirty() override;
	public:
		std::vector<PaperVertex>& V_Ref() {
			return m_V;
		}
		std::vector<uint32_t>& I_Ref() {
			return m_I;
		}
		void BindConvertVertexCoordinate(const ConvertFunc& func) {
			m_ConvertVertexCoordinate = func;
		}
		void BindInteractionActive(const InteractionFunc& func) {
			m_InteractionFunc = func;
		}
	private:
		ConvertFunc m_ConvertVertexCoordinate;
		InteractionFunc m_InteractionFunc;
	};

	class GUI_API TextMeshComponent : public DrawComponent
	{
	protected:
		virtual void Awake()   override;
		virtual void Tick() override;
		virtual void ConvertVertexCoordinate(const JRect& localRect) override;
		virtual bool IsDirty() override;
		virtual void ClearDirty() override;
	public:
		void SetText(const std::wstring& str);
		void SetFont(const std::wstring& path);
		void SetTextHAlign(TextHAlign align);
		void SetTextVAlign(TextVAlign align);
		void SetLineSpacing(float spacing);
		void SetTabSize(float tabSize);
		void SetTextSize(float size);
		const std::wstring& GetText() const {
			return m_Text;
		}
		const std::wstring& GetFont() const {
			return m_FontPath;
		}
		const TextHAlign GetTextHAlign() const {
			return m_HAlign;
		}
		const TextVAlign GetTextVAlign() const {
			return m_VAlign;
		}
		float GetLineSpacing() const {
			return m_LineSpacing;
		}
		float GetTabSize() const {
			return m_TabSize;
		}
		float GetTextSize() const {
			return m_TextSize;
		}
	private:
		enum class UpdateItemState
		{
			Waiting,
			Run,
			Compelete
		};
		struct UpdateItem
		{
			std::wstring Text;
			float        TextSize = 0;
			JFont*       Font     = nullptr;
			std::vector<PaperVertex> v;
			std::vector<uint32_t>    i;
			JRect        localRect;
			TextHAlign   hAlign;
			TextVAlign   vAlign;
			float        lineSpacing;
			float        tabSize;
			UpdateItemState  State = UpdateItemState::Waiting;
		};
		void UpdateTextMesh(UpdateItem& item);
		
	private:
		std::wstring m_FontPath = GUIIF::GetIO().DefaultFont;
		float        m_TextSize = GUIIF::GetStyle().DefaultTextSize;
		JFont*       m_Font     = nullptr;
		std::wstring m_Text;



		// setting
		bool  m_IsTextDirty = true;
		float m_LineSpacing = GUIIF::GetStyle().TextLineSpacing;
		float m_TabSize     = GUIIF::GetStyle().TextTabSize;
		TextHAlign m_HAlign = TextHAlign_Left;
		TextVAlign m_VAlign = TextVAlign_Top;


		
		Concurrency::task_group m_UpdateTextTask;
		UpdateItem m_UpdateItem;
	};








}