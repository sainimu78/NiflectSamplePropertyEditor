#pragma once
#include "Niflect/Serialization/RwTree.h"
//#include <iostream>

namespace RwTree
{
	class CBinaryFormat
	{
		enum class EBinaryFormatObjectType : uint8
		{
			Node,
			Array,
			Value,
		};
	public:
		struct SSSSSSS
		{
			const CRwNode* m_rwNode;
			uint32 m_ownerId;
		};
		//using CIndexingTable = GraphBase::TIndexingMap<SSSSSSS, Niflect::THeapAllocator<GraphBase::TTableItem<SSSSSSS> > >;
		using aaaaa = Niflect::TArrayNif<SSSSSSS>;
		static void BuildRecurs(const CRwNode* rwNode, aaaaa& vecNode, const uint32& ownerIndexingId = INDEX_NONE)
		{
			auto indexingId = static_cast<uint32>(vecNode.size());
			vecNode.push_back({ rwNode, ownerIndexingId });

			auto cnt = rwNode->GetNodesCount();
			for (uint32 idx = 0; idx < cnt; ++idx)
				BuildRecurs(rwNode->GetNode(idx), vecNode, indexingId);
		}
		static uint32 WriteSectionSizeBegin(std::ostream& stm)
		{
			uint32 sectionSizePos = static_cast<uint32>(stm.tellp());
			stm.write(reinterpret_cast<const char*>(&sectionSizePos), sizeof(sectionSizePos));//ռλ
			return sectionSizePos;
		}
		static void WriteSectionSizeEnd(const uint32& sectionSizePos, std::ostream& stm)
		{
			uint32 currentPos = static_cast<uint32>(stm.tellp());
			uint32 sectionSize = currentPos - sectionSizePos - sizeof(sectionSizePos);
			stm.seekp(sectionSizePos);
			stm.write(reinterpret_cast<const char*>(&sectionSize), sizeof(sectionSize));
			stm.seekp(currentPos);
		}
		static void WriteNodesCount(uint32 cnt, std::ostream& stm)
		{
			stm.write(reinterpret_cast<const char*>(&cnt), sizeof(cnt));
		}
		static void A_0(const CRwNode* rwNode, std::ostream& stm)
		{
			auto sectionSizePos = WriteSectionSizeBegin(stm);

			WriteNode(rwNode, stm);

			WriteSectionSizeEnd(sectionSizePos, stm);
		}
		static void A_1(const CRwNode* rwParent, std::ostream& stm)
		{
			auto sectionSizePos = WriteSectionSizeBegin(stm);

			aaaaa vecNode;
			if (!rwParent->IsValue())
			{
				for (uint32 idx = 0; idx < rwParent->GetNodesCount(); ++idx)
					BuildRecurs(rwParent->GetNode(idx), vecNode);
			}

			uint32 cnt = static_cast<uint32>(vecNode.size());
			WriteNodesCount(cnt, stm);
			for (uint32 idx = 0; idx < cnt; ++idx)
				A_0(vecNode[idx].m_rwNode, stm);

			for (uint32 idx = 0; idx < cnt; ++idx)
			{
				auto& item = vecNode[idx];
				stm.write(reinterpret_cast<const char*>(&item.m_ownerId), sizeof(item.m_ownerId));
			}

			WriteSectionSizeEnd(sectionSizePos, stm);
		}
		static void A_2(const CRwNode* rwParent, std::ostream& stm)
		{
			A_0(rwParent, stm);
			A_1(rwParent, stm);
		}
		static uint32 ReadSectionSize(std::istream& stm)
		{
			uint32 sectionSize = 0;
			stm.read(reinterpret_cast<char*>(&sectionSize), sizeof(sectionSize));
			return sectionSize;
		}
		static void SkipSectionOfSize(const uint32 sectionSize, std::istream& stm)
		{
			uint32 currentPos = static_cast<uint32>(stm.tellg());
			stm.seekg(currentPos + sectionSize);
		}
		static uint32 ReadNodesCount(std::istream& stm)
		{
			uint32 cnt = 0;
			stm.read(reinterpret_cast<char*>(&cnt), sizeof(cnt));
			return cnt;
		}
		static void B_0(CRwNode* rwNode, std::istream& stm)
		{
			ReadSectionSize(stm);

			ReadNode(rwNode, stm);
		}
		//static void B_0_Reset(CRwNode* rwNode, std::istream& stm)
		//{
		//	rwNode->ResetType();

		//	B_0(rwNode, stm);
		//}
		static void B_1(CRwNode* rwParent, std::istream& stm)
		{
			ReadSectionSize(stm);

			auto cnt = ReadNodesCount(stm);
			Niflect::TArrayNif<CSharedRwNode> vecRwNode;
			vecRwNode.resize(cnt);
			for (uint32 idx = 0; idx < cnt; ++idx)
			{
				auto rwNode = CreateRwNode();
				B_0(rwNode.Get(), stm);
				vecRwNode[idx] = rwNode;
			}
			for (uint32 idx = 0; idx < cnt; ++idx)
			{
				uint32 ownerId;
				stm.read(reinterpret_cast<char*>(&ownerId), sizeof(ownerId));

				auto rwOwner = rwParent;
				if (ownerId != INDEX_NONE)
					rwOwner = vecRwNode[ownerId].Get();
				rwOwner->AddNode(vecRwNode[idx]);
			}
		}
		static void B_2(CRwNode* rwParent, std::istream& stm)
		{
			B_0(rwParent, stm);
			B_1(rwParent, stm);
		}
		static void TestSkippingAccess(std::istream& stm)
		{
			ReadSectionSize(stm);

			CSharedRwNode rwNode;
			auto cnt = ReadNodesCount(stm);
			for (uint32 idx = 0; idx < cnt; ++idx)
			{
				auto sectionSize = ReadSectionSize(stm);

				if (idx == 5)
				{
					rwNode = CreateRwNode();
					ReadNode(rwNode.Get(), stm);
					break;
				}

				SkipSectionOfSize(sectionSize, stm);
			}
			printf("%s\n", rwNode->GetName().c_str());
		}

	private:
		static void WriteNode(const CRwNode* rwNode, std::ostream& stm)
		{
			auto sectionId = EBinaryFormatObjectType::Node;
			auto isValue = rwNode->IsValue();
			auto isArray = rwNode->IsArray();
			if (isValue)
				sectionId = EBinaryFormatObjectType::Value;
			else if (isArray)
				sectionId = EBinaryFormatObjectType::Array;
			stm.write(reinterpret_cast<const char*>(&sectionId), sizeof(sectionId));

			{
				auto& name = rwNode->GetName();
				auto sz = static_cast<uint32>(name.length());
				stm.write(reinterpret_cast<const char*>(&sz), sizeof(sz));
				if (sz > 0)
					stm.write(name.c_str(), sz);
			}

			if (isValue)
			{
				auto rwValue = rwNode->GetValue();
				auto& type = rwValue->GetType();
				stm.write(reinterpret_cast<const char*>(&type), sizeof(type));
				switch (type)
				{
				case ERwValueType::Bool:
				{
					uint8 data = 0;
					if (rwValue->GetBool())
						data = 1;
					stm.write(reinterpret_cast<char*>(&data), sizeof(data));
					break;
				}
				case ERwValueType::Float:
				{
					auto& data = rwValue->GetFloat();
					stm.write(reinterpret_cast<const char*>(&data), sizeof(data));
					break;
				}
				case ERwValueType::Double:
				{
					auto& data = rwValue->GetDouble();
					stm.write(reinterpret_cast<const char*>(&data), sizeof(data));
					break;
				}
				case ERwValueType::Int32:
				{
					auto& data = rwValue->GetInt32();
					stm.write(reinterpret_cast<const char*>(&data), sizeof(data));
					break;
				}
				case ERwValueType::String:
				{
					auto& str = rwValue->GetString();
					auto sz = static_cast<uint32>(str.length());
					stm.write(reinterpret_cast<const char*>(&sz), sizeof(sz));
					if (sz > 0)
						stm.write(str.c_str(), sz);
					break;
				}
				default:
					ASSERT(false);
					break;
				}
			}
			else if (isArray)
			{
			}
			else
			{
			}
		}

	private:
		static void ReadNode(CRwNode* rwNode, std::istream& stm)
		{
			EBinaryFormatObjectType sectionId;
			stm.read(reinterpret_cast<char*>(&sectionId), sizeof(sectionId));

			{
				Niflect::CString str;
				uint32 sz = 0;
				stm.read(reinterpret_cast<char*>(&sz), sizeof(sz));
				if (sz > 0)
				{
					str.resize(sz);
					stm.read(reinterpret_cast<char*>(&str[0]), sz);
				}
				rwNode->SetName(str);
			}

			switch (sectionId)
			{
			case EBinaryFormatObjectType::Value:
			{
				auto rwValue = rwNode->ToValue();
				auto type = ERwValueType::None;
				stm.read(reinterpret_cast<char*>(&type), sizeof(type));
				switch (type)
				{
				case ERwValueType::Bool:
				{
					uint8 data = 0;
					stm.read(reinterpret_cast<char*>(&data), sizeof(data));
					rwValue->SetBool(data != 0);
					break;
				}
				case ERwValueType::Float:
				{
					float data = 0.0f;
					stm.read(reinterpret_cast<char*>(&data), sizeof(data));
					rwValue->SetFloat(data);
					break;
				}
				case ERwValueType::Double:
				{
					double data = 0.0;
					stm.read(reinterpret_cast<char*>(&data), sizeof(data));
					rwValue->SetDouble(data);
					break;
				}
				case ERwValueType::Int32:
				{
					int32 data = 0;
					stm.read(reinterpret_cast<char*>(&data), sizeof(data));
					rwValue->SetInt32(data);
					break;
				}
				case ERwValueType::String:
				{
					Niflect::CString str;
					uint32 sz = 0;
					stm.read(reinterpret_cast<char*>(&sz), sizeof(sz));
					if (sz > 0)
					{
						str.resize(sz);
						stm.read(reinterpret_cast<char*>(&str[0]), sz);
					}
					rwValue->SetString(str);
					break;
				}
				default:
					ASSERT(false);
					break;
				}
				break;
			}
			case EBinaryFormatObjectType::Array:
			{
				auto rwArray = rwNode->ToArray();
				break;
			}
			case EBinaryFormatObjectType::Node:
			{
				break;
			}
			default:
				break;
			}
		}
		//static EBinaryFormatObjectType GetSectionType(const CRwNode2* rwNode)
		//{
		//	auto sectionId = EBinaryFormatObjectType::Node;
		//	auto isValue = rwNode->IsValue();
		//	auto isArray = rwNode->IsArray();
		//	if (isValue)
		//		sectionId = EBinaryFormatObjectType::Value;
		//	else if (isArray)
		//		sectionId = EBinaryFormatObjectType::Array;
		//	return sectionId;
		//}

	//public:
	//	static void WriteDataRecurs(const CRwNode2* rwNode, std::ostream& stm, uint32 arrayItemIdx = INDEX_NONE)
	//	{
	//		auto sectionSizePos = WriteSectionSizeBegin(stm);

	//		auto sectionId = GetSectionType(rwNode);
	//		stm.write(reinterpret_cast<const char*>(&sectionId), sizeof(sectionId));

	//		if (arrayItemIdx == INDEX_NONE)
	//		{
	//			auto& name = rwNode->GetName();
	//			auto sz = static_cast<uint32>(name.length());
	//			stm.write(reinterpret_cast<const char*>(&sz), sizeof(sz));
	//			if (sz > 0)
	//				stm.write(name.c_str(), sz);
	//		}
	//		else
	//		{
	//			ASSERT(rwNode->GetName().empty());
	//		}
	//		switch (sectionId)
	//		{
	//		case EBinaryFormatObjectType::Value:
	//		{
	//			auto rwValue = rwNode->GetValue();
	//			auto& type = rwValue->GetType();
	//			stm.write(reinterpret_cast<const char*>(&type), sizeof(type));
	//			switch (type)
	//			{
	//			case ERwValueType::Bool:
	//			{
	//				uint8 data = 0;
	//				if (rwValue->GetBool())
	//					data = 1;
	//				stm.write(reinterpret_cast<char*>(&data), sizeof(data));
	//				break;
	//			}
	//			case ERwValueType::Float:
	//			{
	//				auto& data = rwValue->GetFloat();
	//				stm.write(reinterpret_cast<const char*>(&data), sizeof(data));
	//				break;
	//			}
	//			case ERwValueType::Double:
	//			{
	//				auto& data = rwValue->GetDouble();
	//				stm.write(reinterpret_cast<const char*>(&data), sizeof(data));
	//				break;
	//			}
	//			case ERwValueType::Int32:
	//			{
	//				auto& data = rwValue->GetInt32();
	//				stm.write(reinterpret_cast<const char*>(&data), sizeof(data));
	//				break;
	//			}
	//			case ERwValueType::String:
	//			{
	//				auto& str = rwValue->GetString();
	//				auto sz = static_cast<uint32>(str.length());
	//				stm.write(reinterpret_cast<const char*>(&sz), sizeof(sz));
	//				if (sz > 0)
	//					stm.write(str.c_str(), sz);
	//				break;
	//			}
	//			default:
	//				ASSERT(false);
	//				break;
	//			}
	//			break;
	//		}
	//		case EBinaryFormatObjectType::Array:
	//		{
	//			auto rwArray = rwNode->GetArray();
	//			uint32 idxInc = 0;
	//			auto cnt = rwArray->GetItemsCount();
	//			stm.write(reinterpret_cast<const char*>(&cnt), sizeof(cnt));
	//			for (uint32 idx = 0; idx < cnt; ++idx)
	//				WriteDataRecurs(rwArray->GetItem(idx), stm, idxInc++);
	//			break;
	//		}
	//		case EBinaryFormatObjectType::Node:
	//		{
	//			auto cnt = rwNode->GetChildrenCount();
	//			stm.write(reinterpret_cast<const char*>(&cnt), sizeof(cnt));
	//			for (uint32 idx = 0; idx < cnt; ++idx)
	//				WriteDataRecurs(rwNode->GetChild(idx), stm, INDEX_NONE);
	//			break;
	//		}
	//		default:
	//			ASSERT(false);
	//			break;
	//		}

	//		WriteSectionSizeEnd(sectionSizePos, stm);
	//	}
	//	static void ReadDataRecurs(CRwNode2* rwNode, std::istream& stm, uint32 arrayItemIdx = INDEX_NONE)
	//	{
	//		ReadSectionSize(stm);

	//		EBinaryFormatObjectType sectionId;
	//		stm.read(reinterpret_cast<char*>(&sectionId), sizeof(sectionId));

	//		if (arrayItemIdx == INDEX_NONE)
	//		{
	//			Niflect::CString str;
	//			uint32 sz = 0;
	//			stm.read(reinterpret_cast<char*>(&sz), sizeof(sz));
	//			if (sz > 0)
	//			{
	//				str.resize(sz);
	//				stm.read(reinterpret_cast<char*>(&str[0]), sz);
	//			}
	//			rwNode->SetName(str);
	//		}

	//		switch (sectionId)
	//		{
	//		case EBinaryFormatObjectType::Value:
	//		{
	//			auto rwValue = rwNode->ToValue();
	//			auto type = ERwValueType::None;
	//			stm.read(reinterpret_cast<char*>(&type), sizeof(type));
	//			switch (type)
	//			{
	//			case ERwValueType::Bool:
	//			{
	//				uint8 data = 0;
	//				stm.read(reinterpret_cast<char*>(&data), sizeof(data));
	//				rwValue->SetBool(data);
	//				break;
	//			}
	//			case ERwValueType::Float:
	//			{
	//				float data = 0.0f;
	//				stm.read(reinterpret_cast<char*>(&data), sizeof(data));
	//				rwValue->SetFloat(data);
	//				break;
	//			}
	//			case ERwValueType::Double:
	//			{
	//				double data = 0.0;
	//				stm.read(reinterpret_cast<char*>(&data), sizeof(data));
	//				rwValue->SetDouble(data);
	//				break;
	//			}
	//			case ERwValueType::Int32:
	//			{
	//				int32 data = 0;
	//				stm.read(reinterpret_cast<char*>(&data), sizeof(data));
	//				rwValue->SetInt32(data);
	//				break;
	//			}
	//			case ERwValueType::String:
	//			{
	//				Niflect::CString str;
	//				uint32 sz = 0;
	//				stm.read(reinterpret_cast<char*>(&sz), sizeof(sz));
	//				if (sz > 0)
	//				{
	//					str.resize(sz);
	//					stm.read(reinterpret_cast<char*>(&str[0]), sz);
	//				}
	//				rwValue->SetString(str);
	//				break;
	//			}
	//			default:
	//				ASSERT(false);
	//				break;
	//			}
	//			break;
	//		}
	//		case EBinaryFormatObjectType::Array:
	//		{
	//			auto rwArray = rwNode->ToArray();
	//			uint32 cnt = 0;
	//			stm.read(reinterpret_cast<char*>(&cnt), sizeof(cnt));
	//			rwArray->Resize(cnt);
	//			for (uint32 idx = 0; idx < cnt; ++idx)
	//			{
	//				auto rwItem = Niflect::MakeShared<CRwNode2>();
	//				ReadDataRecurs(rwItem.Get(), stm, idx);
	//				rwArray->SetItem(rwItem, idx);
	//			}
	//			break;
	//		}
	//		case EBinaryFormatObjectType::Node:
	//		{
	//			uint32 cnt = 0;
	//			stm.read(reinterpret_cast<char*>(&cnt), sizeof(cnt));
	//			rwNode->Resize(cnt);
	//			for (uint32 idx = 0; idx < cnt; ++idx)
	//			{
	//				auto rwChild = Niflect::MakeShared<CRwNode2>();
	//				ReadDataRecurs(rwChild.Get(), stm, INDEX_NONE);
	//				rwNode->SetChild(rwChild, idx);
	//			}
	//			break;
	//		}
	//		default:
	//			ASSERT(false);
	//			break;
	//		}
	//	}
	};
}