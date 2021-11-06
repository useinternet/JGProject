#pragma once
#include "JGCore.h"
#include "GameObject.h"

namespace JG
{
	class GameObjectFactory : public ObjectFactory<GameObjectFactory, IGameObject, 2>
	{
		friend class GameLogicSystemLayer;
		using CreateFunc = std::function<IGameObject*()>;

		List<IGameObject*>  mReservedDestroyObjects;

		HashSet<Type> mRegisteredSystemTypeSet;
		HashSet<Type> mRegisteredGlobalSystemTypeSet;
		HashSet<Type> mRegisteredNodeTypeSet;
		HashSet<Type> mRegisteredComponentTypeSet;


		Dictionary<Type, CreateFunc> mCreateFuncByObjectType;
		Dictionary<String, Type> mObjectTypeByName;
		Dictionary<u64, Type>    mObjectTypeByID;
	public:
		virtual ~GameObjectFactory() = default;
	public:
		template<class T>
		void RegisterNodeType()
		{
			mRegisteredNodeTypeSet.emplace(Type(TypeID<T>()));
			RegisterType<T>();
		}

		template<class T>
		void RegisterComponentType()
		{
			mRegisteredComponentTypeSet.emplace(Type(TypeID<T>()));
			RegisterType<T>();
		}

		template<class T>
		void RegisterSystemType()
		{
			mRegisteredSystemTypeSet.emplace(Type(TypeID<T>()));
			RegisterType<T>();
		}

		template<class T>
		void RegisterGlobalSystemType()
		{
			mRegisteredGlobalSystemTypeSet.emplace(Type(TypeID<T>()));
			RegisterType<T>();
		}

		Type GetGameObjectType(const String& typeName) const;
		Type GetGameObjectType(u64 typeID) const;
		bool IsGameComponent(const Type& type) const;
		bool IsGameNode(const Type& type)      const;
		bool IsGameSystem(const Type& type)    const;
		bool IsGlobalGameSystem(const Type& type) const;
		bool IsGameComponent(IGameObject* gameObject) const;
		bool IsGameNode(IGameObject* gameObject)      const;
		bool IsGameSystem(IGameObject* gameObject)    const;
		bool IsGlobalGameSystem(IGameObject* gameObject) const;

		IGameObject* CreateObjectByType(const Type& type);

		const HashSet<Type>& GetGameNodeTypeSet() const;
		const HashSet<Type>& GetGameComponentTypeSet() const;
	protected:
		virtual void CreateObjectImpl(IGameObject* gameObject)  override;
		virtual void DestroyObjectImpl(IGameObject* gameObject) override;
		virtual void ReserveDestroyObjectImpl(IGameObject* gameObject) override;
	private:
		EScheduleResult UpdateDestroyObject();
	private:
		template<class T>
		void RegisterType()
		{
			auto type = Type(TypeID<T>());
			if (mCreateFuncByObjectType.find(type) == mCreateFuncByObjectType.end())
			{
				mCreateFuncByObjectType[type] =
					[&]() -> IGameObject*
				{
					return CreateObject<T>();
				};

				mObjectTypeByName[type.GetName()] = type;
				mObjectTypeByID[type.GetID()]     = type;
			}
		}
	};
}