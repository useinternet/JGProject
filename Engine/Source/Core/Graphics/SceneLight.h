#pragma once



namespace JG
{
	namespace Graphics
	{
		enum class ELightType
		{
			PointLight,
			DirectionalLight,
		};


		/// Light ///
		class Light
		{
		public:
			virtual ~Light() = default;
		protected:
			void PushData(List<jbyte>& btData, void* data, u64 size) {
				u64 offset = btData.size();

				btData.resize(offset + size);
				memcpy(&btData[offset], data, size);
			}
		public:
			virtual ELightType GetLightType() const = 0;
			virtual void PushBtData(List<jbyte>& btData) = 0;
			virtual u64  GetBtSize() const = 0;
		};


		class DirectionalLight : public Light
		{
		public:
			virtual ~DirectionalLight() = default;
		public:
			JVector3 Direction;
			JVector3 Color = Color::White();
			f32 Intensity = 1.0f;
		public:
			virtual ELightType GetLightType() const override { return ELightType::DirectionalLight; }
			virtual void PushBtData(List<jbyte>& btData)  override
			{
				PushData(btData, &Direction, sizeof(JVector3));
				PushData(btData, &Intensity, sizeof(f32));
				PushData(btData, &Color, sizeof(JVector3));
			}
			virtual u64 GetBtSize() const override {
				return 28;
			}
		};

		//
		class PointLight : public Light
		{
		public:
			virtual ~PointLight() = default;
		public:
			JVector3 Color;
			JVector3 Position;
			f32 Intensity = 1.0f;
			f32 Range = 0.0f;
			f32 AttRange = 0.0f;
			f32 Att0 = 0.0f;
			f32 Att1 = 0.0f;
			f32 Att2 = 0.0f;
		public:
			virtual ELightType GetLightType() const override { return ELightType::PointLight; }
			virtual void PushBtData(List<jbyte>& btData)  override
			{
				PushData(btData, &Position, sizeof(JVector3));
				PushData(btData, &Range, sizeof(float));
				PushData(btData, &Color, sizeof(JVector3));
				PushData(btData, &AttRange, sizeof(float));
				PushData(btData, &Intensity, sizeof(float));
				PushData(btData, &Att0, sizeof(float));
				PushData(btData, &Att1, sizeof(float));
				PushData(btData, &Att2, sizeof(float));
			}
			virtual u64 GetBtSize() const override {
				return 48;
			}
		};
	}
}