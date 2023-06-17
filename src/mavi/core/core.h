#ifndef VI_CORE_H
#define VI_CORE_H
#include "../config.hpp"
#include <inttypes.h>
#include <thread>
#include <algorithm>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <cstdarg>
#include <cstring>
#include <functional>
#include <vector>
#include <queue>
#include <string>
#include <condition_variable>
#include <atomic>
#include <limits>
#include <array>
#include <list>
#include <sstream>
#ifdef VI_CXX17
#include <charconv>
#endif
#ifdef VI_CXX20
#include <coroutine>
#endif

namespace Mavi
{
	namespace Core
	{
		struct ConcurrentQueue;

		struct Decimal;

		struct Cocontext;

		class Console;

		class Costate;

		class Schema;

		class Stream;

		class ProcessStream;

		class Var;

		typedef uint64_t TaskId;

		enum
		{
			INVALID_TASK_ID = (TaskId)0,
			STACK_SIZE = (size_t)(512 * 1024),
			CHUNK_SIZE = (size_t)2048,
			BLOB_SIZE = (size_t)8192,
			EVENTS_SIZE = (size_t)32
		};

		enum class Timings : uint64_t
		{
			Atomic = 1,
			Pass = 5,
			Frame = 16,
			Mixed = 50,
			FileSystem = 50,
			Networking = 150,
			Intensive = 350,
			Hangup = 5000,
			Infinite = 0
		};

		enum class Deferred : uint8_t
		{
			Pending = 0,
			Waiting = 1,
			Ready = 2
		};

		enum class StdColor
		{
			Black = 0,
			DarkBlue = 1,
			DarkGreen = 2,
			LightBlue = 3,
			DarkRed = 4,
			Magenta = 5,
			Orange = 6,
			LightGray = 7,
			Gray = 8,
			Blue = 9,
			Green = 10,
			Cyan = 11,
			Red = 12,
			Pink = 13,
			Yellow = 14,
			White = 15,
			Zero = 16
		};

		enum class FileMode
		{
			Read_Only,
			Write_Only,
			Append_Only,
			Read_Write,
			Write_Read,
			Read_Append_Write,
			Binary_Read_Only,
			Binary_Write_Only,
			Binary_Append_Only,
			Binary_Read_Write,
			Binary_Write_Read,
			Binary_Read_Append_Write
		};

		enum class FileSeek
		{
			Begin,
			Current,
			End
		};

		enum class VarType : uint8_t
		{
			Null,
			Undefined,
			Object,
			Array,
			Pointer,
			String,
			Binary,
			Integer,
			Number,
			Decimal,
			Boolean
		};

		enum class VarForm
		{
			Dummy,
			Tab_Decrease,
			Tab_Increase,
			Write_Space,
			Write_Line,
			Write_Tab,
		};

		enum class Coactive
		{
			Active,
			Inactive,
			Resumable
		};

		enum class Difficulty
		{
			Coroutine,
			Light,
			Heavy,
			Clock,
			Count
		};

		enum class LogLevel
		{
			Error = 1,
			Warning = 2,
			Info = 3,
			Debug = 4,
			Trace = 5
		};

		enum class LogOption
		{
			Active = 1 << 0,
			Pretty = 1 << 1,
			Async = 1 << 2,
			Dated = 1 << 3,
			ReportSysErrors = 1 << 4
		};

		enum class Optional : char
		{
			Error = -1,
			None = 0,
			Value = 1,
			OK = 1
		};

		enum class Signal
		{
			SIG_INT,
			SIG_ILL,
			SIG_FPE,
			SIG_SEGV,
			SIG_TERM,
			SIG_BREAK,
			SIG_ABRT,
			SIG_BUS,
			SIG_ALRM,
			SIG_HUP,
			SIG_QUIT,
			SIG_TRAP,
			SIG_CONT,
			SIG_STOP,
			SIG_PIPE,
			SIG_CHLD,
			SIG_USR1,
			SIG_USR2
		};

		enum class ParserError
		{
			BadVersion,
			BadDictionary,
			BadNameIndex,
			BadName,
			BadKeyName,
			BadKeyType,
			BadValue,
			BadString,
			BadInteger,
			BadDouble,
			BadBoolean,
			XMLParsingError,
			JSONDocumentEmpty,
			JSONDocumentRootNotSingular,
			JSONValueInvalid,
			JSONObjectMissName,
			JSONObjectMissColon,
			JSONObjectMissCommaOrCurlyBracket,
			JSONArrayMissCommaOrSquareBracket,
			JSONStringUnicodeEscapeInvalidHex,
			JSONStringUnicodeSurrogateInvalid,
			JSONStringEscapeInvalid,
			JSONStringMissQuotationMark,
			JSONStringInvalidEncoding,
			JSONNumberTooBig,
			JSONNumberMissFraction,
			JSONNumberMissExponent,
			JSONTermination,
			JSONUnspecificSyntaxError
		};

		template <typename T, typename = void>
		struct IsIterable : std::false_type { };

		template <typename T>
		struct IsIterable<T, std::void_t<decltype(std::begin(std::declval<T&>())), decltype(std::end(std::declval<T&>()))>> : std::true_type { };

		template <typename T>
		using Unique = T*;

		template <typename T>
		struct Mapping
		{
			T Map;

			~Mapping() = default;
		};

		struct Singletonish { };

		struct VI_OUT MemoryContext
		{
			const char* Source;
			const char* Function;
			const char* TypeName;
			int Line;

			MemoryContext();
			MemoryContext(const char* NewSource, const char* NewFunction, const char* NewTypeName, int NewLine);
		};

		class VI_OUT_TS GlobalAllocator
		{
		public:
			virtual ~GlobalAllocator() = default;
			virtual Unique<void> Allocate(size_t Size) noexcept = 0;
			virtual Unique<void> Allocate(MemoryContext&& Origin, size_t Size) noexcept = 0;
			virtual void Transfer(Unique<void> Address, size_t Size) noexcept = 0;
			virtual void Transfer(Unique<void> Address, MemoryContext&& Origin, size_t Size) noexcept = 0;
			virtual void Free(Unique<void> Address) noexcept = 0;
			virtual void Watch(MemoryContext&& Origin, void* Address) noexcept = 0;
			virtual void Unwatch(void* Address) noexcept = 0;
			virtual void Finalize() noexcept = 0;
			virtual bool IsValid(void* Address) noexcept = 0;
			virtual bool IsFinalizable() noexcept = 0;
		};

		class VI_OUT LocalAllocator
		{
		public:
			virtual ~LocalAllocator() = default;
			virtual Unique<void> Allocate(size_t Size) noexcept = 0;
			virtual void Free(Unique<void> Address) noexcept = 0;
			virtual void Reset() noexcept = 0;
			virtual bool IsValid(void* Address) noexcept = 0;
		};

		class VI_OUT_TS Memory final : public Singletonish
		{
		private:
			struct State
			{
				std::unordered_map<void*, std::pair<MemoryContext, size_t>> Allocations;
				std::mutex Mutex;
			};

		private:
			static GlobalAllocator* Global;
			static State* Context;

		public:
			static Unique<void> Malloc(size_t Size) noexcept;
			static Unique<void> MallocContext(size_t Size, MemoryContext&& Origin) noexcept;
			static void Free(Unique<void> Address) noexcept;
			static void Watch(void* Address, MemoryContext&& Origin) noexcept;
			static void Unwatch(void* Address) noexcept;
			static void Cleanup() noexcept;
			static void SetGlobalAllocator(GlobalAllocator* NewAllocator) noexcept;
			static void SetLocalAllocator(LocalAllocator* NewAllocator) noexcept;
			static bool IsValidAddress(void* Address) noexcept;
			static GlobalAllocator* GetGlobalAllocator() noexcept;
			static LocalAllocator* GetLocalAllocator() noexcept;
		};

		template <typename T>
		class StandardAllocator
		{
		public:
			typedef T value_type;

		public:
			template <typename U>
			struct rebind
			{
				typedef StandardAllocator<U> other;
			};

		public:
			StandardAllocator() = default;
			~StandardAllocator() = default;
			StandardAllocator(const StandardAllocator&) = default;
			value_type* allocate(size_t Count)
			{
				return VI_MALLOC(T, Count * sizeof(T));
			}
			value_type* allocate(size_t Count, const void*)
			{
				return VI_MALLOC(T, Count * sizeof(T));
			}
			void deallocate(value_type* Address, size_t Count)
			{
				VI_FREE((void*)Address);
			}
			size_t max_size() const
			{
				return std::numeric_limits<size_t>::max() / sizeof(T);
			}
			bool operator== (const StandardAllocator&)
			{
				return true;
			}
			bool operator!=(const StandardAllocator&)
			{
				return false;
			}

		public:
			template <typename U>
			StandardAllocator(const StandardAllocator<U>&)
			{
			}
		};

		template <typename T>
		struct AllocationType
		{
#ifdef VI_ALLOCATOR
			using type = StandardAllocator<T>;
#else
			using type = std::allocator<T>;
#endif
		};

		template <typename T, T OffsetBasis, T Prime>
		struct FNV1AHash
		{
			static_assert(std::is_unsigned<T>::value, "Q needs to be unsigned integer");

			inline T operator()(const void* Address, size_t Size) const noexcept
			{
				const auto Data = static_cast<const unsigned char*>(Address);
				auto State = OffsetBasis;
				for (size_t i = 0; i < Size; ++i)
					State = (State ^ (size_t)Data[i]) * Prime;

				return State;
			}
		};

		template <size_t Bits>
		struct FNV1ABits;

		template <>
		struct FNV1ABits<32>
		{
			using type = FNV1AHash<uint32_t, UINT32_C(2166136261), UINT32_C(16777619)>;
		};

		template <>
		struct FNV1ABits<64>
		{
			using type = FNV1AHash<uint64_t, UINT64_C(14695981039346656037), UINT64_C(1099511628211)>;
		};

		template <size_t Bits>
		using FNV1A = typename FNV1ABits<Bits>::type;

		template <typename T>
		struct Hasher
		{
			typedef float argument_type;
			typedef size_t result_type;

			inline result_type operator()(const T& Value) const noexcept
			{
				return std::hash<T>()(Value);
			}
		};

		template <typename T>
		struct EqualTo
		{
			typedef T first_argument_type;
			typedef T second_argument_type;
			typedef bool result_type;

			inline result_type operator()(const T& Left, const T& Right) const noexcept
			{
				return std::equal_to<T>()(Left, Right);
			}
		};

		using String = std::basic_string<std::string::value_type, std::string::traits_type, typename AllocationType<typename std::string::value_type>::type>;
		using WideString = std::basic_string<std::wstring::value_type, std::wstring::traits_type, typename AllocationType<typename std::wstring::value_type>::type>;
		using StringStream = std::basic_stringstream<std::string::value_type, std::string::traits_type, typename AllocationType<typename std::string::value_type>::type>;
		using WideStringStream = std::basic_stringstream<std::wstring::value_type, std::wstring::traits_type, typename AllocationType<typename std::wstring::value_type>::type>;

		template <>
		struct EqualTo<String>
		{
			typedef String first_argument_type;
			typedef String second_argument_type;
			typedef bool result_type;
			using is_transparent = void;

			inline result_type operator()(const String& Left, const String& Right) const noexcept
			{
				return Left == Right;
			}
			inline result_type operator()(const char* Left, const String& Right) const noexcept
			{
				return Right.compare(Left) == 0;
			}
			inline result_type operator()(const String& Left, const char* Right) const noexcept
			{
				return Left.compare(Right) == 0;
			}
#ifdef VI_CXX17
			inline result_type operator()(const std::string_view& Left, const String& Right) const noexcept
			{
				return Left == Right;
			}
			inline result_type operator()(const String& Left, const std::string_view& Right) const noexcept
			{
				return Left == Right;
			}
#endif
		};

		template <>
		struct Hasher<String>
		{
			typedef float argument_type;
			typedef size_t result_type;
			using is_transparent = void;

			inline result_type operator()(const char* Value) const noexcept
			{
				return FNV1A<8 * sizeof(size_t)>()(Value, strlen(Value));
			}
			inline result_type operator()(const String& Value) const noexcept
			{
				return FNV1A<8 * sizeof(size_t)>()(Value.c_str(), Value.size());
			}
#ifdef VI_CXX17
			inline result_type operator()(const std::string_view& Value) const noexcept
			{
				return FNV1A<8 * sizeof(size_t)>()(Value.data(), Value.size());
			}
#endif
		};

		template <>
		struct Hasher<WideString>
		{
			typedef float argument_type;
			typedef size_t result_type;

			inline result_type operator()(const WideString& Value) const noexcept
			{
				return FNV1A<8 * sizeof(std::size_t)>()(Value.c_str(), Value.size());
			}
		};

		template <typename T>
		using Vector = std::vector<T, typename AllocationType<T>::type>;

		template <typename T>
		using LinkedList = std::list<T, typename AllocationType<T>::type>;

		template <typename T>
		using SingleQueue = std::queue<T, std::deque<T, typename AllocationType<T>::type>>;

		template <typename T>
		using DoubleQueue = std::deque<T, typename AllocationType<T>::type>;

		template <typename K, typename Hash = Hasher<K>, typename KeyEqual = EqualTo<K>>
		using UnorderedSet = std::unordered_set<K, Hash, KeyEqual, typename AllocationType<typename std::unordered_set<K>::value_type>::type>;

		template <typename K, typename V, typename Hash = Hasher<K>, typename KeyEqual = EqualTo<K>>
		using UnorderedMap = std::unordered_map<K, V, Hash, KeyEqual, typename AllocationType<typename std::unordered_map<K, V>::value_type>::type>;

		template <typename K, typename V, typename Hash = Hasher<K>, typename KeyEqual = EqualTo<K>>
		using UnorderedMultiMap = std::unordered_multimap<K, V, Hash, KeyEqual, typename AllocationType<typename std::unordered_multimap<K, V>::value_type>::type>;

		template <typename K, typename V, typename Comparator = typename std::map<K, V>::key_compare>
		using OrderedMap = std::map<K, V, Comparator, typename AllocationType<typename std::map<K, V>::value_type>::type>;

		typedef std::function<void()> TaskCallback;
		typedef std::function<String(const String&)> SchemaNameCallback;
		typedef std::function<void(VarForm, const char*, size_t)> SchemaWriteCallback;
		typedef std::function<bool(char*, size_t)> SchemaReadCallback;
		typedef std::function<bool()> ActivityCallback;
		typedef void(*SignalCallback)(int);

		namespace Allocators
		{
			class VI_OUT_TS DebugAllocator final : public GlobalAllocator
			{
			public:
				struct VI_OUT_TS TracingBlock
				{
					std::thread::id Thread;
					std::string TypeName;
					MemoryContext Origin;
					time_t Time;
					size_t Size;
					bool Active;
					bool Static;

					TracingBlock();
					TracingBlock(const char* NewTypeName, MemoryContext&& NewOrigin, time_t NewTime, size_t NewSize, bool IsActive, bool IsStatic);
				};

			private:
				std::unordered_map<void*, TracingBlock> Blocks;
				std::recursive_mutex Mutex;

			public:
				~DebugAllocator() override = default;
				Unique<void> Allocate(size_t Size) noexcept override;
				Unique<void> Allocate(MemoryContext&& Origin, size_t Size) noexcept override;
				void Free(Unique<void> Address) noexcept override;
				void Transfer(Unique<void> Address, size_t Size) noexcept override;
				void Transfer(Unique<void> Address, MemoryContext&& Origin, size_t Size) noexcept override;
				void Watch(MemoryContext&& Origin, void* Address) noexcept override;
				void Unwatch(void* Address) noexcept override;
				void Finalize() noexcept override;
				bool IsValid(void* Address) noexcept override;
				bool IsFinalizable() noexcept override;
				bool Dump(void* Address);
				bool FindBlock(void* Address, TracingBlock* Output);
				const std::unordered_map<void*, TracingBlock>& GetBlocks() const;
			};

			class VI_OUT_TS DefaultAllocator final : public GlobalAllocator
			{
			public:
				~DefaultAllocator() override = default;
				Unique<void> Allocate(size_t Size) noexcept override;
				Unique<void> Allocate(MemoryContext&& Origin, size_t Size) noexcept override;
				void Free(Unique<void> Address) noexcept override;
				void Transfer(Unique<void> Address, size_t Size) noexcept override;
				void Transfer(Unique<void> Address, MemoryContext&& Origin, size_t Size) noexcept override;
				void Watch(MemoryContext&& Origin, void* Address) noexcept override;
				void Unwatch(void* Address) noexcept override;
				void Finalize() noexcept override;
				bool IsValid(void* Address) noexcept override;
				bool IsFinalizable() noexcept override;
			};

			class VI_OUT_TS CachedAllocator final : public GlobalAllocator
			{
			private:
				struct PageCache;

				using PageGroup = std::vector<PageCache*>;

				struct PageAddress
				{
					PageCache* Cache;
					void* Address;
				};

				struct PageCache
				{
					std::vector<PageAddress*> Addresses;
					PageGroup& Page;
					int64_t Timing;
					size_t Capacity;

					inline PageCache(PageGroup& NewPage, int64_t Time, size_t Size) : Page(NewPage), Timing(Time), Capacity(Size)
					{
						Addresses.resize(Capacity);
					}
					~PageCache() = default;
				};

			private:
				std::unordered_map<size_t, PageGroup> Pages;
				std::recursive_mutex Mutex;
				uint64_t MinimalLifeTime;
				double ElementsReducingFactor;
				size_t ElementsReducingBase;
				size_t ElementsPerAllocation;

			public:
				CachedAllocator(uint64_t MinimalLifeTimeMs = 2000, size_t MaxElementsPerAllocation = 1024, size_t ElementsReducingBaseBytes = 300, double ElementsReducingFactorRate = 5.0);
				~CachedAllocator() noexcept override;
				Unique<void> Allocate(size_t Size) noexcept override;
				Unique<void> Allocate(MemoryContext&& Origin, size_t Size) noexcept override;
				void Free(Unique<void> Address) noexcept override;
				void Transfer(Unique<void> Address, size_t Size) noexcept override;
				void Transfer(Unique<void> Address, MemoryContext&& Origin, size_t Size) noexcept override;
				void Watch(MemoryContext&& Origin, void* Address) noexcept override;
				void Unwatch(void* Address) noexcept override;
				void Finalize() noexcept override;
				bool IsValid(void* Address) noexcept override;
				bool IsFinalizable() noexcept override;

			private:
				PageCache* GetPageCache(size_t Size);
				int64_t GetClock();
				size_t GetElementsCount(PageGroup& Page, size_t Size);
			};

			class VI_OUT LinearAllocator final : public LocalAllocator
			{
			private:
				struct Region
				{
					char* FreeAddress;
					char* BaseAddress;
					Region* UpperAddress;
					Region* LowerAddress;
					size_t Size;
				};

			private:
				Region* Top;
				Region* Bottom;
				size_t LatestSize;
				size_t Sizing;

			public:
				LinearAllocator(size_t Size);
				~LinearAllocator() noexcept override;
				Unique<void> Allocate(size_t Size) noexcept override;
				void Free(Unique<void> Address) noexcept override;
				void Reset() noexcept override;
				bool IsValid(void* Address) noexcept override;
				size_t GetLeftovers() const noexcept;

			private:
				void NextRegion(size_t Size) noexcept;
				void FlushRegions() noexcept;
			};

			class VI_OUT StackAllocator final : public LocalAllocator
			{
			private:
				struct Region
				{
					char* FreeAddress;
					char* BaseAddress;
					Region* UpperAddress;
					Region* LowerAddress;
					size_t Size;
				};

			private:
				Region* Top;
				Region* Bottom;
				size_t Sizing;

			public:
				StackAllocator(size_t Size);
				~StackAllocator() noexcept override;
				Unique<void> Allocate(size_t Size) noexcept override;
				void Free(Unique<void> Address) noexcept override;
				void Reset() noexcept override;
				bool IsValid(void* Address) noexcept override;
				size_t GetLeftovers() const noexcept;

			private:
				void NextRegion(size_t Size) noexcept;
				void FlushRegions() noexcept;
			};
		}

		namespace Exceptions
		{
			class ParserException : public std::exception
			{
			public:
				String Info;
				ParserError Type;
				int Offset;

			public:
				ParserException(ParserError NewType);
				ParserException(ParserError NewType, int NewOffset);
				ParserException(ParserError NewType, int NewOffset, const char* NewMessage);
				const char* what() const noexcept override;
			};
		}

		class VI_OUT StackTrace
		{
		public:
			struct VI_OUT Frame
			{
				String Code;
				String Function;
				String File;
				uint32_t Line;
				uint32_t Column;
				void* Handle;
				bool Native;
			};

		public:
			typedef Vector<Frame> StackPtr;

		private:
			StackPtr Frames;

		public:
			StackTrace(size_t Skips = 0, size_t MaxDepth = 64);
			StackPtr::const_iterator begin() const;
			StackPtr::const_iterator end() const;
			StackPtr::const_reverse_iterator rbegin() const;
			StackPtr::const_reverse_iterator rend() const;
			explicit operator bool() const;
			const StackPtr& Range() const;
			bool IsEmpty() const;
			size_t Size() const;
		};

		class VI_OUT_TS ErrorHandling final : public Singletonish
		{
		public:
			struct VI_OUT Details
			{
				struct
				{
					const char* File = nullptr;
					int Line = 0;
				} Origin;
				
				struct
				{
					LogLevel Level = LogLevel::Info;
					bool Fatal = false;
				} Type;

				struct
				{
					char Data[BLOB_SIZE] = { '\0' };
					char Date[64] = { '\0' };
					size_t Size = 0;
				} Message;
			};

			struct VI_OUT Tick
			{
				bool IsCounting;

				Tick(bool Active) noexcept;
				Tick(const Tick& Other) = delete;
				Tick(Tick&& Other) noexcept;
				~Tick() noexcept;
				Tick& operator =(const Tick& Other) = delete;
				Tick& operator =(Tick&& Other) noexcept;
			};

		private:
			struct State
			{
				std::function<void(Details&)> Callback;
				uint32_t Flags = (uint32_t)LogOption::Pretty;
			};

		private:
			static State* Context;

		public:
			static void Panic(int Line, const char* Source, const char* Function, const char* Condition, const char* Format, ...) noexcept;
			static void Assert(int Line, const char* Source, const char* Function, const char* Condition, const char* Format, ...) noexcept;
			static void Message(LogLevel Level, int Line, const char* Source, const char* Format, ...) noexcept;
			static void Pause() noexcept;
			static void Cleanup() noexcept;
			static void SetCallback(const std::function<void(Details&)>& Callback) noexcept;
			static void SetFlag(LogOption Option, bool Active) noexcept;
			static bool HasFlag(LogOption Option) noexcept;
			static bool HasCallback() noexcept;
			static String GetStackTrace(size_t Skips, size_t MaxFrames = 64) noexcept;
			static String GetMeasureTrace() noexcept;
			static Tick Measure(const char* File, const char* Function, int Line, uint64_t ThresholdMS) noexcept;
			static void MeasureLoop() noexcept;
			static const char* GetMessageType(const Details& Base) noexcept;
			static StdColor GetMessageColor(const Details& Base) noexcept;
			static String GetMessageText(const Details& Base) noexcept;

		private:
			static void Enqueue(Details&& Data) noexcept;
			static void Dispatch(Details& Data) noexcept;
			static void Colorify(Console* Base, Details& Data) noexcept;
		};

		template <typename T>
		class Bitmask
		{
			static_assert(std::is_integral<T>::value, "value should be of integral type to have bitmask applications");

		public:
			static T Mark(T Other)
			{
				return Other | (T)Highbit<T>();
			}
			static T Unmark(T Other)
			{
				return Other & (T)Lowbit<T>();
			}
			static T Value(T Other)
			{
				return Other & (T)Lowbit<T>();
			}
			static bool IsMarked(T Other)
			{
				return (bool)(Other & (T)Highbit<T>());
			}

		private:
			template <typename Q>
			static typename std::enable_if<sizeof(Q) == sizeof(uint64_t), uint64_t>::type Highbit()
			{
				return 0x8000000000000000;
			}
			template <typename Q>
			static typename std::enable_if<sizeof(Q) < sizeof(uint64_t), uint32_t>::type Highbit()
			{
				return 0x80000000;
			}
			template <typename Q>
			static typename std::enable_if<sizeof(Q) == sizeof(uint64_t), uint64_t>::type Lowbit()
			{
				return 0x7FFFFFFFFFFFFFFF;
			}
			template <typename Q>
			static typename std::enable_if<sizeof(Q) < sizeof(uint64_t), uint32_t>::type Lowbit()
			{
				return 0x7FFFFFFF;
			}
		};

		template <typename V>
		class Option
		{
			static_assert(!std::is_same<V, Optional>::value, "value type should not be optional type");

		private:
			char Value[sizeof(V)];
			bool Empty;

		public:
			Option(Optional Other) : Empty(true)
			{
				VI_ASSERT(Other != Optional::Error, "only none and value are accepted for this constructor");
			}
			Option(const V& Other) : Empty(false)
			{
				new (Value) V(Other);
			}
			Option(V&& Other) noexcept : Empty(false)
			{
				new (Value) V(std::move(Other));
			}
			Option(const Option& Other) : Empty(Other.Empty)
			{
				if (!Empty)
					new (Value) V(*(V*)Other.Value);
			}
			Option(Option&& Other) noexcept : Empty(Other.Empty)
			{
				memcpy(Value, Other.Value, sizeof(Value));
				Other.Empty = true;
			}
			~Option()
			{
				if (!Empty)
					((V*)Value)->~V();
			}
			Option& operator= (Optional Value)
			{
				VI_ASSERT(Value != Optional::Error, "only none and value are accepted for this constructor");
				this->~Option();
				Empty = true;
				return *this;
			}
			Option& operator= (const V& Other)
			{
				this->~Option();
				new (Value) V(Other);
				Empty = false;
				return *this;
			}
			Option& operator= (V&& Other) noexcept
			{
				this->~Option();
				new (Value) V(std::move(Other));
				Empty = false;
				return *this;
			}
			Option& operator= (const Option& Other)
			{
				if (this == &Other)
					return *this;

				this->~Option();
				Empty = Other.Empty;
				if (!Empty)
					new (Value) V(*(V*)Other.Value);

				return *this;
			}
			Option& operator= (Option&& Other) noexcept
			{
				if (this == &Other)
					return *this;

				this->~Option();
				memcpy(Value, Other.Value, sizeof(Value));
				Empty = Other.Empty;
				Other.Empty = true;
				return *this;
			}
			const V& OrPanic(const char* Message) const
			{
				VI_ASSERT(Message != nullptr && Message[0] != '\0', "panic case message should be set");
				VI_PANIC(IsValue(), "panic is caused by %s", Message);
				return *(V*)Value;
			}
			V&& OrPanic(const char* Message)
			{
				VI_ASSERT(Message != nullptr && Message[0] != '\0', "panic case message should be set");
				VI_PANIC(IsValue(), "panic is caused by %s", Message);
				return std::move(*(V*)Value);
			}
			const V& Or(const V& IfNone) const
			{
				return IsValue() ? *(V*)Value : IfNone;
			}
			V&& Or(V&& IfNone)
			{
				if (!IsValue())
					*this = std::move(IfNone);
				return std::move(*(V*)Value);
			}
			const V& operator* () const
			{
				VI_PANIC(IsValue(), "option does not contain any value");
				return *(V*)Value;
			}
			V&& operator* ()
			{
				VI_PANIC(IsValue(), "option does not contain any value");
				return std::move(*(V*)Value);
			}
			const typename std::remove_pointer<V>::type* operator-> () const
			{
				return GetConstReference<V>();
			}
			typename std::remove_pointer<V>::type* operator-> ()
			{
				return GetReference<V>();
			}
			explicit operator bool() const
			{
				return IsValue();
			}
			explicit operator Optional() const
			{
				return IsValue() ? Optional::Value : Optional::None;
			}
			void Reset()
			{
				this->~Option();
				Empty = true;
			}
			bool IsNone() const
			{
				return Empty;
			}
			bool IsValue() const
			{
				return !Empty;
			}

		private:
			template <typename Q>
			inline typename std::enable_if<std::is_pointer<Q>::value, Q>::type GetReference()
			{
				Q&& Data = this->operator*();
				return Data;
			}
			template <typename Q>
			inline typename std::enable_if<!std::is_pointer<Q>::value, Q*>::type GetReference()
			{
				Q&& Data = this->operator*();
				return &Data;
			}
			template <typename Q>
			inline typename std::enable_if<std::is_pointer<Q>::value, const Q>::type GetConstReference() const
			{
				const Q& Data = this->operator*();
				return Data;
			}
			template <typename Q>
			inline typename std::enable_if<!std::is_pointer<Q>::value, const Q*>::type GetConstReference() const
			{
				const Q& Data = this->operator*();
				return &Data;
			}
		};

		template <>
		class Option<void>
		{
		private:
			bool Empty;

		public:
			Option(Optional Value) : Empty(Value == Optional::None)
			{
				VI_ASSERT(Value != Optional::Error, "only none and value are accepted for this constructor");
			}
			Option(const Option&) = default;
			Option(Option&& Other) = default;
			~Option() = default;
			Option& operator= (Optional Value)
			{
				VI_ASSERT(Value != Optional::Error, "only none and value are accepted for this constructor");
				Empty = (Value == Optional::None);
				return *this;
			}
			Option& operator= (const Option&) = default;
			Option& operator= (Option&& Other) = default;
			bool OrPanic(const char* Message) const
			{
				VI_ASSERT(Message != nullptr, "panic case message should be set");
				VI_PANIC(IsValue(), "%s", Message);
				return true;
			}
			bool OrPanic(const char* Message)
			{
				VI_ASSERT(Message != nullptr, "panic case message should be set");
				VI_PANIC(IsValue(), "%s", Message);
				return true;
			}
			explicit operator bool() const
			{
				return IsValue();
			}
			explicit operator Optional() const
			{
				return IsValue() ? Optional::Value : Optional::None;
			}
			void Reset()
			{
				Empty = true;
			}
			bool IsNone() const
			{
				return Empty;
			}
			bool IsValue() const
			{
				return !Empty;
			}
		};

		template <typename V, typename E>
		class Expects
		{
			static_assert(!std::is_same<E, void>::value, "error type should not be void");
			static_assert(!std::is_same<E, V>::value, "error type should not be value type");

		private:
			using ValueSize = std::integral_constant<std::size_t, std::max(sizeof(V), sizeof(E))>;
			char Value[ValueSize::value];
			char Status;

		public:
			Expects(const V& Other) : Status(1)
			{
				new (Value) V(Other);
			}
			Expects(V&& Other) noexcept : Status(1)
			{
				new (Value) V(std::move(Other));
			}
			Expects(const E& Other) noexcept : Status(-1)
			{
				new (Value) E(Other);
			}
			Expects(E&& Other) noexcept : Status(-1)
			{
				new (Value) E(std::move(Other));
			}
			Expects(const Expects& Other) : Status(Other.Status)
			{
				if (Status > 0)
					new (Value) V(*(V*)Other.Value);
				else if (Status < 0)
					new (Value) E(*(E*)Other.Value);
			}
			Expects(Expects&& Other) noexcept : Status(Other.Status)
			{
				Other.Status = 0;
				if (Status > 0)
					memcpy(Value, Other.Value, sizeof(V));
				else if (Status < 0)
					memcpy(Value, Other.Value, sizeof(E));
			}
			~Expects()
			{
				if (Status > 0)
					((V*)Value)->~V();
				else if (Status < 0)
					((E*)Value)->~E();
			}
			Expects& operator= (const V& Other)
			{
				this->~Expects();
				new (Value) V(Other);
				Status = 1;
				return **this;
			}
			Expects& operator= (V&& Other) noexcept
			{
				this->~Expects();
				new (Value) V(std::move(Other));
				Status = 1;
				return *this;
			}
			Expects& operator= (const E& Other)
			{
				this->~Expects();
				new (Value) E(Other);
				Status = -1;
				return **this;
			}
			Expects& operator= (E&& Other) noexcept
			{
				this->~Expects();
				new (Value) E(std::move(Other));
				Status = -1;
				return *this;
			}
			Expects& operator= (const Expects& Other)
			{
				if (this == &Other)
					return *this;

				this->~Expects();
				Status = Other.Status;
				if (Status > 0)
					new (Value) V(*(V*)Other.Value);
				else if (Status < 0)
					new (Value) E(*(E*)Other.Value);

				return *this;
			}
			Expects& operator= (Expects&& Other) noexcept
			{
				if (this == &Other)
					return *this;

				this->~Expects();
				Status = Other.Status;
				Other.Status = 0;
				if (Status > 0)
					memcpy(Value, Other.Value, sizeof(V));
				else if (Status < 0)
					memcpy(Value, Other.Value, sizeof(E));
				return *this;
			}
			const V& OrPanic(const char* Message) const
			{
				VI_ASSERT(Message != nullptr && Message[0] != '\0', "panic case message should be set");
				VI_PANIC(IsValue(), "%s caused by %s", Message, GetErrorText<E>().c_str());
				return *(V*)Value;
			}
			V&& OrPanic(const char* Message)
			{
				VI_ASSERT(Message != nullptr && Message[0] != '\0', "panic case message should be set");
				VI_PANIC(IsValue(), "%s caused by %s", Message, GetErrorText<E>().c_str());
				return std::move(*(V*)Value);
			}
			const V& Or(const V& IfNone) const
			{
				return IsValue() ? *(V*)Value : IfNone;
			}
			V&& Or(V&& IfNone)
			{
				if (!IsValue())
					*this = std::move(IfNone);
				return std::move(*(V*)Value);
			}
			const E& Error() const
			{
				VI_ASSERT(IsError(), "outcome does not contain any errors");
				return *(E*)Value;
			}
			E&& Error()
			{
				VI_ASSERT(IsError(), "outcome does not contain any errors");
				return std::move(*(E*)Value);
			}
			void Report(const char* Message) const
			{
				VI_ASSERT(Message != nullptr && Message[0] != '\0', "report case message should be set");
				if (IsError())
					VI_ERR("%s caused by %s", Message, GetErrorText<E>().c_str());
			}
			String What() const
			{
				VI_ASSERT(!IsValue(), "outcome does not contain any errors");
				return GetErrorText<E>();
			}
			explicit operator bool() const
			{
				return IsValue();
			}
			explicit operator Optional() const
			{
				return (Optional)Status;
			}
			const V& operator* () const
			{
				VI_PANIC(IsValue(), "error caused by %s", GetErrorText<E>().c_str());
				return *(V*)Value;
			}
			V&& operator* ()
			{
				VI_PANIC(IsValue(), "error caused by %s", GetErrorText<E>().c_str());
				return std::move(*(V*)Value);
			}
			const typename std::remove_pointer<V>::type* operator-> () const
			{
				return GetConstReference<V>();
			}
			typename std::remove_pointer<V>::type* operator-> ()
			{
				return GetReference<V>();
			}
			void Reset()
			{
				this->~Expects();
				Status = 0;
			}
			bool IsNone() const
			{
				return !Status;
			}
			bool IsValue() const
			{
				return Status > 0;
			}
			bool IsError() const
			{
				return Status < 0;
			}

		private:
			template <typename Q>
			inline typename std::enable_if<std::is_pointer<Q>::value, Q>::type GetReference()
			{
				Q&& Data = this->operator*();
				return Data;
			}
			template <typename Q>
			inline typename std::enable_if<!std::is_pointer<Q>::value, Q*>::type GetReference()
			{
				Q&& Data = this->operator*();
				return &Data;
			}
			template <typename Q>
			inline typename std::enable_if<std::is_pointer<Q>::value, const Q>::type GetConstReference() const
			{
				const Q& Data = this->operator*();
				return Data;
			}
			template <typename Q>
			inline typename std::enable_if<!std::is_pointer<Q>::value, const Q*>::type GetConstReference() const
			{
				const Q& Data = this->operator*();
				return &Data;
			}
			template <typename Q>
			inline typename std::enable_if<std::is_base_of<std::exception, Q>::value, String>::type GetErrorText() const
			{
				return String(IsError() ?((Q*)Value)->what() : "*no value stored*");
			}
			template <typename Q>
			inline typename std::enable_if<std::is_same<std::error_code, Q>::value, std::string>::type GetErrorText() const
			{
				return IsError() ? ((Q*)Value)->message() : std::string("*no value stored*");
			}
			template <typename Q>
			inline typename std::enable_if<std::is_same<std::error_condition, Q>::value, std::string>::type GetErrorText() const
			{
				return IsError() ? ((Q*)Value)->message() : std::string("*no value stored*");
			}
			template <typename Q>
			inline typename std::enable_if<std::is_same<std::string, Q>::value, std::string>::type GetErrorText() const
			{
				return IsError() ? *(Q*)Value : Q("*no value stored*");
			}
#ifdef VI_ALLOCATOR
			template <typename Q>
			inline typename std::enable_if<std::is_same<String, Q>::value, String>::type GetErrorText() const
			{
				return IsError() ? *(Q*)Value : Q("*no value stored*");
			}
#endif
			template <typename Q>
			inline typename std::enable_if<!std::is_base_of<std::exception, Q>::value && !std::is_same<std::error_code, Q>::value && !std::is_same<std::error_condition, Q>::value && !std::is_same<std::string, Q>::value && !std::is_same<String, Q>::value, String>::type GetErrorText() const
			{
				return String(IsError() ? "*error cannot be shown*" : "*no value stored*");
			}
		};

		template <typename E>
		class Expects<void, E>
		{
			static_assert(!std::is_same<E, void>::value, "error type should not be void");

		private:
			char Value[sizeof(E)];
			char Status;

		public:
			Expects(Optional Type) : Status(1)
			{
				VI_ASSERT(Type == Optional::Value, "only value is accepted for this constructor");
			}
			Expects(const E& Other) noexcept : Status(-1)
			{
				new (Value) E(Other);
			}
			Expects(E&& Other) noexcept : Status(-1)
			{
				new (Value) E(std::move(Other));
			}
			Expects(const Expects& Other) : Status(Other.Status)
			{
				if (Status < 0)
					new (Value) E(*(E*)Other.Value);
			}
			Expects(Expects&& Other) noexcept : Status(Other.Status)
			{
				Other.Status = 0;
				if (Status < 0)
					memcpy(Value, Other.Value, sizeof(E));
			}
			~Expects()
			{
				if (Status < 0)
					((E*)Value)->~E();
			}
			Expects& operator= (Optional Type)
			{
				VI_ASSERT(Type == Optional::Value, "only value is accepted for this operator");
				this->~Expects();
				Status = 1;
				return **this;
			}
			Expects& operator= (const E& Other)
			{
				this->~Expects();
				new (Value) E(Other);
				Status = -1;
				return **this;
			}
			Expects& operator= (E&& Other) noexcept
			{
				this->~Expects();
				new (Value) E(std::move(Other));
				Status = -1;
				return *this;
			}
			Expects& operator= (const Expects& Other)
			{
				if (this == &Other)
					return *this;

				this->~Expects();
				Status = Other.Status;
				if (Status < 0)
					new (Value) E(*(E*)Other.Value);

				return *this;
			}
			Expects& operator= (Expects&& Other) noexcept
			{
				if (this == &Other)
					return *this;

				this->~Expects();
				Status = Other.Status;
				Other.Status = 0;
				if (Status < 0)
					memcpy(Value, Other.Value, sizeof(E));
				return *this;
			}
			void OrPanic(const char* Message) const
			{
				VI_ASSERT(Message != nullptr && Message[0] != '\0', "panic case message should be set");
				VI_PANIC(IsValue(), "%s caused by %s", Message, GetErrorText<E>().c_str());
			}
			const E& Error() const
			{
				VI_ASSERT(IsError(), "outcome does not contain any errors");
				return *(E*)Value;
			}
			E&& Error()
			{
				VI_ASSERT(IsError(), "outcome does not contain any errors");
				return std::move(*(E*)Value);
			}
			void Report(const char* Message) const
			{
				VI_ASSERT(Message != nullptr && Message[0] != '\0', "report case message should be set");
				if (IsError())
					VI_ERR("%s caused by %s", Message, GetErrorText<E>().c_str());
			}
			String What() const
			{
				VI_ASSERT(!IsValue(), "outcome does not contain any errors");
				return GetErrorText<E>();
			}
			explicit operator bool() const
			{
				return IsValue();
			}
			explicit operator Optional() const
			{
				return (Optional)Status;
			}
			void Reset()
			{
				this->~Expects();
				Status = 0;
			}
			bool IsNone() const
			{
				return !Status;
			}
			bool IsValue() const
			{
				return Status > 0;
			}
			bool IsError() const
			{
				return Status < 0;
			}

		private:
			template <typename Q>
			inline typename std::enable_if<std::is_base_of<std::exception, Q>::value, String>::type GetErrorText() const
			{
				return String(IsError() ? ((Q*)Value)->what() : "*no value stored*");
			}
			template <typename Q>
			inline typename std::enable_if<std::is_same<std::error_code, Q>::value, std::string>::type GetErrorText() const
			{
				return IsError() ? ((Q*)Value)->message() : std::string("*no value stored*");
			}
			template <typename Q>
			inline typename std::enable_if<std::is_same<std::error_condition, Q>::value, std::string>::type GetErrorText() const
			{
				return IsError() ? ((Q*)Value)->message() : std::string("*no value stored*");
			}
			template <typename Q>
			inline typename std::enable_if<std::is_same<std::string, Q>::value, std::string>::type GetErrorText() const
			{
				return IsError() ? *(Q*)Value : Q("*no value stored*");
			}
#ifdef VI_ALLOCATOR
			template <typename Q>
			inline typename std::enable_if<std::is_same<String, Q>::value, String>::type GetErrorText() const
			{
				return IsError() ? *(Q*)Value : Q("*no value stored*");
			}
#endif
			template <typename Q>
			inline typename std::enable_if<!std::is_base_of<std::exception, Q>::value && !std::is_same<std::error_code, Q>::value && !std::is_same<std::error_condition, Q>::value && !std::is_same<std::string, Q>::value && !std::is_same<String, Q>::value, String>::type GetErrorText() const
			{
				return String(IsError() ? "*error cannot be shown*" : "*no value stored*");
			}
		};

		template <typename V>
		using Expected = Expects<V, std::error_condition>;

		struct VI_OUT Coroutine
		{
			friend Costate;

		private:
			std::atomic<Coactive> State;
			std::atomic<int> Dead;
			TaskCallback Callback;
			Cocontext* Slave;
			Costate* Master;

		public:
			TaskCallback Return;

		private:
			Coroutine(Costate* Base, const TaskCallback& Procedure) noexcept;
			Coroutine(Costate* Base, TaskCallback&& Procedure) noexcept;
			~Coroutine() noexcept;
		};

		struct VI_OUT Decimal
		{
		private:
			DoubleQueue<char> Source;
			int Length;
			char Sign;
			bool Invalid;

		public:
			Decimal() noexcept;
			Decimal(const char* Value) noexcept;
			Decimal(const String& Value) noexcept;
			Decimal(int32_t Value) noexcept;
			Decimal(uint32_t Value) noexcept;
			Decimal(int64_t Value) noexcept;
			Decimal(uint64_t Value) noexcept;
			Decimal(float Value) noexcept;
			Decimal(double Value) noexcept;
			Decimal(const Decimal& Value) noexcept;
			Decimal(Decimal&& Value) noexcept;
			Decimal& Truncate(int Value);
			Decimal& Round(int Value);
			Decimal& Trim();
			Decimal& Unlead();
			Decimal& Untrail();
			bool IsNaN() const;
			bool IsZero() const;
			bool IsZeroOrNaN() const;
			bool IsPositive() const;
			bool IsNegative() const;
			double ToDouble() const;
			float ToFloat() const;
			int64_t ToInt64() const;
			uint64_t ToUInt64() const;
			String ToString() const;
			String Exp() const;
			int Decimals() const;
			int Ints() const;
			int Size() const;
			Decimal operator -() const;
			Decimal& operator *=(const Decimal& V);
			Decimal& operator /=(const Decimal& V);
			Decimal& operator +=(const Decimal& V);
			Decimal& operator -=(const Decimal& V);
			Decimal& operator= (const Decimal& Value) noexcept;
			Decimal& operator= (Decimal&& Value) noexcept;
			Decimal& operator++ (int);
			Decimal& operator++ ();
			Decimal& operator-- (int);
			Decimal& operator-- ();
			bool operator== (const Decimal& Right) const;
			bool operator!= (const Decimal& Right) const;
			bool operator> (const Decimal& Right) const;
			bool operator>= (const Decimal& Right) const;
			bool operator< (const Decimal& Right) const;
			bool operator<= (const Decimal& Right) const;
			explicit operator double() const
			{
				return ToDouble();
			}
			explicit operator float() const
			{
				return ToFloat();
			}
			explicit operator int64_t() const
			{
				return ToInt64();
			}
			explicit operator uint64_t() const
			{
				return ToUInt64();
			}
			explicit operator String() const
			{
				return ToString();
			}

		public:
			VI_OUT friend Decimal operator + (const Decimal& Left, const Decimal& Right);
			VI_OUT friend Decimal operator - (const Decimal& Left, const Decimal& Right);
			VI_OUT friend Decimal operator * (const Decimal& Left, const Decimal& Right);
			VI_OUT friend Decimal operator / (const Decimal& Left, const Decimal& Right);
			VI_OUT friend Decimal operator % (const Decimal& Left, const Decimal& Right);

		public:
			static Decimal Divide(const Decimal& Left, const Decimal& Right, int Precision);
			static Decimal NaN();

		private:
			static Decimal Sum(const Decimal& Left, const Decimal& Right);
			static Decimal Subtract(const Decimal& Left, const Decimal& Right);
			static Decimal Multiply(const Decimal& Left, const Decimal& Right);
			static int CompareNum(const Decimal& Left, const Decimal& Right);
			static int CharToInt(const char& Value);
			static char IntToChar(const int& Value);
		};

		struct VI_OUT Variant
		{
			friend Schema;
			friend Var;

		private:
			union Tag
			{
				char String[32];
				char* Pointer;
				int64_t Integer;
				double Number;
				bool Boolean;
			} Value;

		private:
			VarType Type;
			uint32_t Size;

		public:
			Variant() noexcept;
			Variant(const Variant& Other) noexcept;
			Variant(Variant&& Other) noexcept;
			~Variant() noexcept;
			bool Deserialize(const String& Value, bool Strict = false);
			String Serialize() const;
			String GetBlob() const;
			Decimal GetDecimal() const;
			void* GetPointer() const;
			const char* GetString() const;
			unsigned char* GetBinary() const;
			int64_t GetInteger() const;
			double GetNumber() const;
			bool GetBoolean() const;
			VarType GetType() const;
			size_t GetSize() const;
			Variant& operator= (const Variant& Other) noexcept;
			Variant& operator= (Variant&& Other) noexcept;
			bool operator== (const Variant& Other) const;
			bool operator!= (const Variant& Other) const;
			explicit operator bool() const;
			bool IsString(const char* Value) const;
			bool IsObject() const;
			bool IsEmpty() const;
			bool Is(VarType Value) const;

		private:
			Variant(VarType NewType) noexcept;
			bool Same(const Variant& Value) const;
			void Copy(const Variant& Other);
			void Move(Variant&& Other);
			void Free();

		private:
			static size_t GetMaxSmallStringSize();
		};

		typedef Vector<Variant> VariantList;
		typedef Vector<Schema*> SchemaList;
		typedef UnorderedMap<String, Variant> VariantArgs;
		typedef UnorderedMap<String, Schema*> SchemaArgs;

		struct VI_OUT TextSettle
		{
			size_t Start = 0;
			size_t End = 0;
			bool Found = false;
		};

		struct VI_OUT FileState
		{
			size_t Size = 0;
			size_t Links = 0;
			size_t Permissions = 0;
			size_t Document = 0;
			size_t Device = 0;
			size_t UserId = 0;
			size_t GroupId = 0;
			int64_t LastAccess = 0;
			int64_t LastPermissionChange = 0;
			int64_t LastModified = 0;
			bool Exists = false;
		};

		struct VI_OUT Timeout
		{
			std::chrono::microseconds Expires;
			TaskCallback Callback;
			Difficulty Type;
			TaskId Id;
			bool Alive;

			Timeout(const TaskCallback& NewCallback, const std::chrono::microseconds& NewTimeout, TaskId NewId, bool NewAlive, Difficulty NewType) noexcept;
			Timeout(TaskCallback&& NewCallback, const std::chrono::microseconds& NewTimeout, TaskId NewId, bool NewAlive, Difficulty NewType) noexcept;
			Timeout(const Timeout& Other) noexcept;
			Timeout(Timeout&& Other) noexcept;
			Timeout& operator= (const Timeout& Other) noexcept;
			Timeout& operator= (Timeout&& Other) noexcept;
		};

		struct VI_OUT FileEntry
		{
			size_t Size = 0;
			int64_t LastModified = 0;
			int64_t CreationTime = 0;
			bool IsReferenced = false;
			bool IsDirectory = false;
			bool IsExists = false;
		};

		struct VI_OUT ChildProcess
		{
			friend class OS;

		private:
			bool Valid = false;
#ifdef VI_MICROSOFT
			void* Process = nullptr;
			void* Thread = nullptr;
			void* Job = nullptr;
#else
			pid_t Process;
#endif

		public:
			int64_t GetPid();
		};

		struct VI_OUT DateTime
		{
		private:
			std::chrono::system_clock::duration Time;
			struct tm DateValue;
			bool DateRebuild;

		public:
			DateTime() noexcept;
			DateTime(uint64_t Seconds) noexcept;
			DateTime(const DateTime& Value) noexcept;
			DateTime& operator= (const DateTime& Other) noexcept;
			void operator +=(const DateTime& Right);
			void operator -=(const DateTime& Right);
			bool operator >=(const DateTime& Right);
			bool operator <=(const DateTime& Right);
			bool operator >(const DateTime& Right);
			bool operator <(const DateTime& Right);
			bool operator ==(const DateTime& Right);
			String Format(const String& Value);
			String Date(const String& Value);
			String Iso8601();
			DateTime Now();
			DateTime FromNanoseconds(uint64_t Value);
			DateTime FromMicroseconds(uint64_t Value);
			DateTime FromMilliseconds(uint64_t Value);
			DateTime FromSeconds(uint64_t Value);
			DateTime FromMinutes(uint64_t Value);
			DateTime FromHours(uint64_t Value);
			DateTime FromDays(uint64_t Value);
			DateTime FromWeeks(uint64_t Value);
			DateTime FromMonths(uint64_t Value);
			DateTime FromYears(uint64_t Value);
			DateTime operator +(const DateTime& Right) const;
			DateTime operator -(const DateTime& Right) const;
			DateTime& SetDateSeconds(uint64_t Value, bool NoFlush = true);
			DateTime& SetDateMinutes(uint64_t Value, bool NoFlush = true);
			DateTime& SetDateHours(uint64_t Value, bool NoFlush = true);
			DateTime& SetDateDay(uint64_t Value, bool NoFlush = true);
			DateTime& SetDateWeek(uint64_t Value, bool NoFlush = true);
			DateTime& SetDateMonth(uint64_t Value, bool NoFlush = true);
			DateTime& SetDateYear(uint64_t Value, bool NoFlush = true);
			uint64_t DateSecond();
			uint64_t DateMinute();
			uint64_t DateHour();
			uint64_t DateDay();
			uint64_t DateWeek();
			uint64_t DateMonth();
			uint64_t DateYear();
			uint64_t Nanoseconds();
			uint64_t Microseconds();
			uint64_t Milliseconds();
			uint64_t Seconds();
			uint64_t Minutes();
			uint64_t Hours();
			uint64_t Days();
			uint64_t Weeks();
			uint64_t Months();
			uint64_t Years();

		private:
			void Rebuild();

		public:
			static String FetchWebDateGMT(int64_t TimeStamp);
			static String FetchWebDateTime(int64_t TimeStamp);
			static bool FetchWebDateGMT(char* Buffer, size_t Length, int64_t Time);
			static bool FetchWebDateTime(char* Buffer, size_t Length, int64_t Time);
			static void FetchDateTime(char* Buffer, size_t Length, int64_t Time);
			static int64_t ParseWebDate(const char* Date);
		};

		struct VI_OUT Stringify
		{
		public:
			static String& EscapePrint(String& Other);
			static String& Escape(String& Other);
			static String& Unescape(String& Other);
			static String& ToUpper(String& Other);
			static String& ToLower(String& Other);
			static String& Clip(String& Other, size_t Length);
			static String& Compress(String& Other, const char* SpaceIfNotFollowedOrPrecededByOf, const char* NotInBetweenOf, size_t Start = 0U);
			static String& ReplaceOf(String& Other, const char* Chars, const char* To, size_t Start = 0U);
			static String& ReplaceNotOf(String& Other, const char* Chars, const char* To, size_t Start = 0U);
			static String& ReplaceGroups(String& Other, const String& FromRegex, const String& To);
			static String& Replace(String& Other, const String& From, const String& To, size_t Start = 0U);
			static String& Replace(String& Other, const char* From, const char* To, size_t Start = 0U);
			static String& Replace(String& Other, const char& From, const char& To, size_t Position = 0U);
			static String& Replace(String& Other, const char& From, const char& To, size_t Position, size_t Count);
			static String& ReplacePart(String& Other, size_t Start, size_t End, const String& Value);
			static String& ReplacePart(String& Other, size_t Start, size_t End, const char* Value);
			static String& ReplaceStartsWithEndsOf(String& Other, const char* Begins, const char* EndsOf, const String& With, size_t Start = 0U);
			static String& ReplaceInBetween(String& Other, const char* Begins, const char* Ends, const String& With, bool Recursive, size_t Start = 0U);
			static String& ReplaceNotInBetween(String& Other, const char* Begins, const char* Ends, const String& With, bool Recursive, size_t Start = 0U);
			static String& ReplaceParts(String& Other, Vector<std::pair<String, TextSettle>>& Inout, const String& With, const std::function<char(const String&, char, int)>& Surrounding = nullptr);
			static String& ReplaceParts(String& Other, Vector<TextSettle>& Inout, const String& With, const std::function<char(char, int)>& Surrounding = nullptr);
			static String& RemovePart(String& Other, size_t Start, size_t End);
			static String& Reverse(String& Other);
			static String& Reverse(String& Other, size_t Start, size_t End);
			static String& Substring(String& Other, const TextSettle& Result);
			static String& Splice(String& Other, size_t Start, size_t End);
			static String& Trim(String& Other);
			static String& TrimStart(String& Other);
			static String& TrimEnd(String& Other);
			static String& Fill(String& Other, const char& Char);
			static String& Fill(String& Other, const char& Char, size_t Count);
			static String& Fill(String& Other, const char& Char, size_t Start, size_t Count);
			static String& Append(String& Other, const char* Format, ...);
			static String& Erase(String& Other, size_t Position);
			static String& Erase(String& Other, size_t Position, size_t Count);
			static String& EraseOffsets(String& Other, size_t Start, size_t End);
			static String& EvalEnvs(String& Other, const String& Net, const String& Dir);
			static Vector<std::pair<String, TextSettle>> FindInBetween(const String& Other, const char* Begins, const char* Ends, const char* NotInSubBetweenOf, size_t Offset = 0U);
			static Vector<std::pair<String, TextSettle>> FindStartsWithEndsOf(const String& Other, const char* Begins, const char* EndsOf, const char* NotInSubBetweenOf, size_t Offset = 0U);
			static TextSettle ReverseFind(const String& Other, const String& Needle, size_t Offset = 0U);
			static TextSettle ReverseFind(const String& Other, const char* Needle, size_t Offset = 0U);
			static TextSettle ReverseFind(const String& Other, const char& Needle, size_t Offset = 0U);
			static TextSettle ReverseFindUnescaped(const String& Other, const char& Needle, size_t Offset = 0U);
			static TextSettle ReverseFindOf(const String& Other, const String& Needle, size_t Offset = 0U);
			static TextSettle ReverseFindOf(const String& Other, const char* Needle, size_t Offset = 0U);
			static TextSettle Find(const String& Other, const String& Needle, size_t Offset = 0U);
			static TextSettle Find(const String& Other, const char* Needle, size_t Offset = 0U);
			static TextSettle Find(const String& Other, const char& Needle, size_t Offset = 0U);
			static TextSettle FindUnescaped(const String& Other, const char& Needle, size_t Offset = 0U);
			static TextSettle FindOf(const String& Other, const String& Needle, size_t Offset = 0U);
			static TextSettle FindOf(const String& Other, const char* Needle, size_t Offset = 0U);
			static TextSettle FindNotOf(const String& Other, const String& Needle, size_t Offset = 0U);
			static TextSettle FindNotOf(const String& Other, const char* Needle, size_t Offset = 0U);
			static bool IsPrecededBy(const String& Other, size_t At, const char* Of);
			static bool IsFollowedBy(const String& Other, size_t At, const char* Of);
			static bool StartsWith(const String& Other, const String& Value, size_t Offset = 0U);
			static bool StartsWith(const String& Other, const char* Value, size_t Offset = 0U);
			static bool StartsOf(const String& Other, const char* Value, size_t Offset = 0U);
			static bool StartsNotOf(const String& Other, const char* Value, size_t Offset = 0U);
			static bool EndsWith(const String& Other, const String& Value);
			static bool EndsOf(const String& Other, const char* Value);
			static bool EndsNotOf(const String& Other, const char* Value);
			static bool EndsWith(const String& Other, const char* Value);
			static bool EndsWith(const String& Other, const char& Value);
			static bool HasInteger(const String& Other);
			static bool HasNumber(const String& Other);
			static bool HasDecimal(const String& Other);
			static String Text(const char* Format, ...);
			static WideString ToWide(const String& Other);
			static Vector<String> Split(const String& Other, const String& With, size_t Start = 0U);
			static Vector<String> Split(const String& Other, char With, size_t Start = 0U);
			static Vector<String> SplitMax(const String& Other, char With, size_t MaxCount, size_t Start = 0U);
			static Vector<String> SplitOf(const String& Other, const char* With, size_t Start = 0U);
			static Vector<String> SplitNotOf(const String& Other, const char* With, size_t Start = 0U);
			static bool IsDigit(char Char);
			static bool IsAlphabetic(char Char);
			static int CaseCompare(const char* Value1, const char* Value2);
			static int Match(const char* Pattern, const char* Text);
			static int Match(const char* Pattern, size_t Length, const char* Text);
			static void ConvertToWide(const char* Input, size_t InputSize, wchar_t* Output, size_t OutputSize);
		};

		class VI_OUT_TS Var
		{
		public:
			class VI_OUT Set
			{
			public:
				static Unique<Schema> Auto(Variant&& Value);
				static Unique<Schema> Auto(const Variant& Value);
				static Unique<Schema> Auto(const Core::String& Value, bool Strict = false);
				static Unique<Schema> Null();
				static Unique<Schema> Undefined();
				static Unique<Schema> Object();
				static Unique<Schema> Array();
				static Unique<Schema> Pointer(void* Value);
				static Unique<Schema> String(const Core::String& Value);
				static Unique<Schema> String(const char* Value, size_t Size);
				static Unique<Schema> Binary(const Core::String& Value);
				static Unique<Schema> Binary(const unsigned char* Value, size_t Size);
				static Unique<Schema> Binary(const char* Value, size_t Size);
				static Unique<Schema> Integer(int64_t Value);
				static Unique<Schema> Number(double Value);
				static Unique<Schema> Decimal(const Core::Decimal& Value);
				static Unique<Schema> Decimal(Core::Decimal&& Value);
				static Unique<Schema> DecimalString(const Core::String& Value);
				static Unique<Schema> Boolean(bool Value);
			};

		public:
			static Variant Auto(const Core::String& Value, bool Strict = false);
			static Variant Null();
			static Variant Undefined();
			static Variant Object();
			static Variant Array();
			static Variant Pointer(void* Value);
			static Variant String(const Core::String& Value);
			static Variant String(const char* Value, size_t Size);
			static Variant Binary(const Core::String& Value);
			static Variant Binary(const unsigned char* Value, size_t Size);
			static Variant Binary(const char* Value, size_t Size);
			static Variant Integer(int64_t Value);
			static Variant Number(double Value);
			static Variant Decimal(const Core::Decimal& Value);
			static Variant Decimal(Core::Decimal&& Value);
			static Variant DecimalString(const Core::String& Value);
			static Variant Boolean(bool Value);
		};

		class VI_OUT_TS OS
		{
		public:
			class VI_OUT CPU
			{
			public:
				enum class Arch
				{
					X64,
					ARM,
					Itanium,
					X86,
					Unknown,
				};

				enum class Endian
				{
					Little,
					Big,
				};

				enum class Cache
				{
					Unified,
					Instruction,
					Data,
					Trace,
				};

				struct QuantityInfo
				{
					uint32_t Logical;
					uint32_t Physical;
					uint32_t Packages;
				};

				struct CacheInfo
				{
					size_t Size;
					size_t LineSize;
					uint8_t Associativity;
					Cache Type;
				};

			public:
				static QuantityInfo GetQuantityInfo();
				static CacheInfo GetCacheInfo(unsigned int level);
				static Arch GetArch() noexcept;
				static Endian GetEndianness() noexcept;
				static size_t GetFrequency() noexcept;

			public:
				template <typename T>
				static typename std::enable_if<std::is_arithmetic<T>::value, T>::type SwapEndianness(T Value)
				{
					union U
					{
						T Value;
						std::array<std::uint8_t, sizeof(T)> Data;
					} From, To;

					From.Value = Value;
					std::reverse_copy(From.Data.begin(), From.Data.end(), To.Data.begin());
					return To.Value;
				}
				template <typename T>
				static typename std::enable_if<std::is_arithmetic<T>::value, T>::type ToEndianness(Endian Type, T Value)
				{
					return GetEndianness() == Type ? Value : SwapEndianness(Value);
				}
			};

			class VI_OUT Directory
			{
			public:
				static bool IsExists(const char* Path);
				static Expected<void> SetWorking(const char* Path);
				static Expected<void> Patch(const String& Path);
				static Expected<void> Scan(const String& Path, Vector<std::pair<String, FileEntry>>* Entries);
				static Expected<void> Create(const char* Path);
				static Expected<void> Remove(const char* Path);
				static Expected<String> GetModule();
				static Expected<String> GetWorking();
				static Vector<String> GetMounts();
			};

			class VI_OUT File
			{
			public:
				static bool IsExists(const char* Path);
				static int Compare(const String& FirstPath, const String& SecondPath);
				static uint64_t GetHash(const String& Data);
				static uint64_t GetIndex(const char* Data, size_t Size);
				static Expected<void> Write(const String& Path, const char* Data, size_t Length);
				static Expected<void> Write(const String& Path, const String& Data);
				static Expected<void> Move(const char* From, const char* To);
				static Expected<void> Copy(const char* From, const char* To);
				static Expected<void> Remove(const char* Path);
				static Expected<void> Close(Unique<void> Stream);
				static Expected<void> GetState(const String& Path, FileEntry* Output);
				static Expected<size_t> Join(const String& To, const Vector<String>& Paths);
				static Expected<FileState> GetProperties(const char* Path);
				static Expected<FileEntry> GetState(const String& Path);
				static Expected<Unique<Stream>> OpenJoin(const String& Path, const Vector<String>& Paths);
				static Expected<Unique<Stream>> OpenArchive(const String& Path, size_t UnarchivedMaxSize = 128 * 1024 * 1024);
				static Expected<Unique<Stream>> Open(const String& Path, FileMode Mode, bool Async = false);
				static Expected<Unique<FILE>> Open(const char* Path, const char* Mode);
				static Expected<Unique<unsigned char>> ReadChunk(Stream* Stream, size_t Length);
				static Expected<Unique<unsigned char>> ReadAll(const String& Path, size_t* ByteLength);
				static Expected<Unique<unsigned char>> ReadAll(Stream* Stream, size_t* ByteLength);
				static Expected<String> ReadAsString(const String& Path);
				static Expected<Vector<String>> ReadAsArray(const String& Path);

			public:
				template <size_t Size>
				static constexpr uint64_t GetIndex(const char Source[Size]) noexcept
				{
					uint64_t Result = 0xcbf29ce484222325;
					for (size_t i = 0; i < Size; i++)
					{
						Result ^= Source[i];
						Result *= 1099511628211;
					}

					return Result;
				}
			};

			class VI_OUT Path
			{
			public:
				static bool IsRemote(const char* Path);
				static bool IsRelative(const char* Path);
				static bool IsAbsolute(const char* Path);
				static const char* GetFilename(const char* Path);
				static const char* GetExtension(const char* Path);
				static String GetDirectory(const char* Path, size_t Level = 0);
				static String GetNonExistant(const String& Path);
				static Expected<String> Resolve(const char* Path);
				static Expected<String> Resolve(const String& Path, const String& Directory, bool EvenIfExists);
				static Expected<String> ResolveDirectory(const char* Path);
				static Expected<String> ResolveDirectory(const String& Path, const String& Directory, bool EvenIfExists);
			};

			class VI_OUT Net
			{
			public:
				static bool GetETag(char* Buffer, size_t Length, FileEntry* Resource);
				static bool GetETag(char* Buffer, size_t Length, int64_t LastModified, size_t ContentLength);
				static socket_t GetFd(FILE* Stream);
			};

			class VI_OUT Process
			{
            public:
                struct VI_OUT ArgsContext
                {
				public:
                    UnorderedMap<String, String> Base;
                    
				public:
					ArgsContext(int Argc, char** Argv, const String& WhenNoValue = "1") noexcept;
					void ForEach(const std::function<void(const String&, const String&)>& Callback) const;
					bool IsEnabled(const String& Option, const String& Shortcut = "") const;
					bool IsDisabled(const String& Option, const String& Shortcut = "") const;
					bool Has(const String& Option, const String& Shortcut = "") const;
					String& Get(const String& Option, const String& Shortcut = "");
					String& GetIf(const String& Option, const String& Shortcut, const String& WhenEmpty);
					String& GetAppPath();
                    
                private:
					bool IsTrue(const String& Value) const;
					bool IsFalse(const String& Value) const;
                };
                
			public:
				static void Abort();
				static void Interrupt();
				static void Exit(int Code);
				static bool SetSignalCallback(Signal Type, SignalCallback Callback);
				static bool SetSignalDefault(Signal Type);
				static bool SetSignalIgnore(Signal Type);
				static int GetSignalId(Signal Type);
				static int ExecutePlain(const String& Command);
				static bool Spawn(const String& Path, const Vector<String>& Params, ChildProcess* Result);
				static bool Await(ChildProcess* Process, int* ExitCode);
				static bool Free(ChildProcess* Process);
				static Expected<Unique<ProcessStream>> ExecuteWriteOnly(const String& Command);
				static Expected<Unique<ProcessStream>> ExecuteReadOnly(const String& Command);
				static Expected<String> GetEnv(const String& Name);
                static String GetThreadId(const std::thread::id& Id);
                static UnorderedMap<String, String> GetArgs(int Argc, char** Argv, const String& WhenNoValue = "1");
			};

			class VI_OUT Symbol
			{
			public:
				static Expected<Unique<void>> Load(const String& Path = "");
				static Expected<Unique<void>> LoadFunction(void* Handle, const String& Name);
				static Expected<void> Unload(void* Handle);
			};

			class VI_OUT Input
			{
			public:
				static bool Text(const String& Title, const String& Message, const String& DefaultInput, String* Result);
				static bool Password(const String& Title, const String& Message, String* Result);
				static bool Save(const String& Title, const String& DefaultPath, const String& Filter, const String& FilterDescription, String* Result);
				static bool Open(const String& Title, const String& DefaultPath, const String& Filter, const String& FilterDescription, bool Multiple, String* Result);
				static bool Folder(const String& Title, const String& DefaultPath, String* Result);
				static bool Color(const String& Title, const String& DefaultHexRGB, String* Result);
			};

			class VI_OUT Error
			{
			public:
				static int Get(bool Clear = true);
				static void Clear();
				static bool Occurred();
				static bool IsError(int Code);
				static std::error_condition GetCondition();
				static std::error_condition GetCondition(int Code);
				static std::error_condition GetConditionOr(std::errc Code = std::errc::invalid_argument);
				static String GetName(int Code);
			};
		};

		class VI_OUT_TS Composer : public Singletonish
		{
		private:
			struct State
			{
				UnorderedMap<uint64_t, std::pair<uint64_t, void*>> Factory;
				std::mutex Mutex;
			};

		private:
			static State* Context;

		public:
			static UnorderedSet<uint64_t> Fetch(uint64_t Id) noexcept;
			static bool Pop(const String& Hash) noexcept;
			static void Cleanup() noexcept;

		private:
			static void Push(uint64_t TypeId, uint64_t Tag, void* Callback) noexcept;
			static void* Find(uint64_t TypeId) noexcept;

		public:
			template <typename T, typename... Args>
			static Unique<T> Create(const String& Hash, Args... Data) noexcept
			{
				return Create<T, Args...>(VI_HASH(Hash), Data...);
			}
			template <typename T, typename... Args>
			static Unique<T> Create(uint64_t Id, Args... Data) noexcept
			{
				void* (*Callable)(Args...) = nullptr;
				reinterpret_cast<void*&>(Callable) = Find(Id);

				if (!Callable)
					return nullptr;

				return (T*)Callable(Data...);
			}
			template <typename T, typename... Args>
			static void Push(uint64_t Tag) noexcept
			{
				auto Callable = &Composer::Callee<T, Args...>;
				void* Result = reinterpret_cast<void*&>(Callable);
				Push(T::GetTypeId(), Tag, Result);
			}

		private:
			template <typename T, typename... Args>
			static Unique<void> Callee(Args... Data) noexcept
			{
				return (void*)new T(Data...);
			}
		};

		template <typename T>
		class Reference
		{
		private:
			std::atomic<uint32_t> __vcnt = 1;

		public:
			void operator delete(void* Ptr) noexcept
			{
				if (Ptr != nullptr)
				{
					auto Handle = (T*)Ptr;
					VI_ASSERT(Handle->__vcnt <= 1, "address at 0x%" PRIXPTR " is still in use but destructor has been called by delete as %s at %s()", Ptr, typeid(T).name(), __func__);
					VI_FREE(Ptr);
				}
			}
			void* operator new(size_t Size) noexcept
			{
				return (void*)VI_MALLOC(T, Size);
			}
			bool IsMarkedRef() const noexcept
			{
				return Bitmask<uint32_t>::IsMarked(__vcnt.load());
			}
			uint32_t GetRefCount() const noexcept
			{
				return Bitmask<uint32_t>::Value(__vcnt.load());
			}
			void MarkRef() noexcept
			{
				__vcnt = Bitmask<uint32_t>::Mark(__vcnt.load());
			}
			void AddRef() noexcept
			{
				__vcnt = Bitmask<uint32_t>::Value(__vcnt.load()) + 1;
			}
			void Release() noexcept
			{
				__vcnt = Bitmask<uint32_t>::Unmark(__vcnt.load());
				VI_ASSERT(__vcnt > 0 && Memory::IsValidAddress((void*)(T*)this), "address at 0x%" PRIXPTR " has already been released as %s at %s()", (void*)this, typeid(T).name(), __func__);
				if (!--__vcnt)
					delete (T*)this;
			}
		};

		template <typename T>
		class Singleton : public Reference<T>
		{
		private:
			enum class Action
			{
				Destroy,
				Restore,
				Create,
				Store,
				Fetch
			};

		public:
			Singleton() noexcept
			{
				UpdateInstance((T*)this, Action::Store);
			}
			virtual ~Singleton() noexcept
			{
				if (UpdateInstance(nullptr, Action::Fetch) == (T*)this)
					UpdateInstance(nullptr, Action::Restore);
			}

		public:
			static bool CleanupInstance() noexcept
			{
				return !UpdateInstance(nullptr, Action::Destroy);
			}
			static bool HasInstance() noexcept
			{
				return UpdateInstance(nullptr, Action::Fetch) != nullptr;
			}
			static T* Get() noexcept
			{
				return UpdateInstance(nullptr, Action::Create);
			}

		private:
			template <typename Q>
			static typename std::enable_if<std::is_default_constructible<Q>::value, void>::type CreateInstance(Q*& Instance) noexcept
			{
				if (!Instance)
					Instance = new Q();
			}
			template <typename Q>
			static typename std::enable_if<!std::is_default_constructible<Q>::value, void>::type CreateInstance(Q*& Instance) noexcept
			{
			}
			static T* UpdateInstance(T* Other, Action Type) noexcept
			{
				static T* Instance = nullptr;
				switch (Type)
				{
					case Action::Destroy:
						VI_RELEASE(Instance);
						return nullptr;
					case Action::Restore:
						Instance = nullptr;
						return nullptr;
					case Action::Create:
						CreateInstance<T>(Instance);
						return Instance;
					case Action::Fetch:
						return Instance;
					case Action::Store:
						if (Other == Instance)
							return Instance;

						VI_RELEASE(Instance);		
						Instance = Other;
						return Instance;
					default:
						return nullptr;
				}
			}
		};

		template <typename T>
		class UPtr
		{
		private:
			T* Pointer;

		public:
			UPtr(T* NewPointer) noexcept : Pointer(NewPointer)
			{
			}
			UPtr(const UPtr& Other) noexcept = delete;
			UPtr(UPtr&& Other) noexcept : Pointer(Other.Pointer)
			{
				Other.Pointer = nullptr;
			}
			~UPtr()
			{
				Cleanup<T>();
			}
			UPtr& operator= (const UPtr& Other) noexcept = delete;
			UPtr& operator= (UPtr&& Other) noexcept
			{
				if (this == &Other)
					return *this;

				Cleanup<T>();
				Pointer = Other.Pointer;
				Other.Pointer = nullptr;
				return *this;
			}
			explicit operator bool() const
			{
				return Pointer != nullptr;
			}
			T* operator-> ()
			{
				VI_ASSERT(Pointer != nullptr, "unique pointer invalid access");
				return Pointer;
			}
			T* operator* ()
			{
				return Pointer;
			}
			T* OrPanic(const char* Message)
			{
				VI_ASSERT(Message != nullptr && Message[0] != '\0', "panic case message should be set");
				VI_PANIC(Pointer != nullptr, "panic is caused by %s", Message);
				return Pointer;
			}
			Unique<T> Reset()
			{
				T* Result = Pointer;
				Pointer = nullptr;
				return Result;
			}

		private:
			template <typename Q>
			inline typename std::enable_if<std::is_trivially_default_constructible<Q>::value && !std::is_base_of<Reference<Q>, Q>::value, void>::type Cleanup()
			{
				VI_FREE(Pointer);
				Pointer = nullptr;
			}
			template <typename Q>
			inline typename std::enable_if<!std::is_trivially_default_constructible<Q>::value && !std::is_base_of<Reference<Q>, Q>::value, void>::type Cleanup()
			{
				VI_DELETE(T, Pointer);
				Pointer = nullptr;
			}
			template <typename Q>
			inline typename std::enable_if<!std::is_trivially_default_constructible<Q>::value && std::is_base_of<Reference<Q>, Q>::value, void>::type Cleanup()
			{
				VI_CLEAR(Pointer);
			}
		};

		template <typename T>
		class UMutex
		{
			static_assert(std::is_same<std::mutex, T>::value || std::is_same<std::recursive_mutex, T>::value, "unique mutex type should be one of: std::mutex, std::recursive_mutex");

		private:
			T& Mutex;
			bool Owns;

		public:
			UMutex(T& NewMutex) noexcept : Mutex(NewMutex), Owns(true)
			{
				Mutex.lock();
			}
			UMutex(const UMutex& Other) noexcept = delete;
			UMutex(UMutex&& Other) noexcept = delete;
			~UMutex()
			{
				if (Owns)
					Mutex.unlock();
			}
			UMutex& operator= (const UMutex& Other) noexcept = delete;
			UMutex& operator= (UMutex&& Other) noexcept = delete;
			inline void Negate()
			{
				if (Owns)
					Mutex.unlock();
				else
					Mutex.lock();
				Owns = !Owns;
			}

		public:
			template <typename F>
			inline void Negated(F&& Callback)
			{
				if (Owns)
				{
					Mutex.unlock();
					Callback();
					Mutex.lock();
				}
				else
				{
					Mutex.lock();
					Callback();
					Mutex.unlock();
				}
			}
		};

		template <typename T>
		class UAlloc
		{
			static_assert(std::is_base_of<LocalAllocator, T>::value, "unique allocator type should be based on local allocator");

		private:
			T& Allocator;

		public:
			UAlloc(T& NewAllocator) noexcept : Allocator(NewAllocator)
			{
				Memory::SetLocalAllocator(&NewAllocator);
			}
			UAlloc(const UAlloc& Other) noexcept = delete;
			UAlloc(UAlloc&& Other) noexcept = delete;
			~UAlloc()
			{
				Memory::SetLocalAllocator(nullptr);
				Allocator.Reset();
			}
			UAlloc& operator= (const UAlloc& Other) noexcept = delete;
			UAlloc& operator= (UAlloc&& Other) noexcept = delete;
		};

		class VI_OUT_TS Console final : public Singleton<Console>
		{
		public:
			struct ColorToken
			{
				StdColor Color;
				const char* Token;
				char First;
				size_t Size;

				ColorToken(StdColor BaseColor, const char* Name) : Color(BaseColor), Token(Name), First(Name ? *Name : '\0'), Size(Name ? strlen(Name) : 0)
				{
				}
			};

		private:
			enum class Mode
			{
				Attached,
				Allocated,
				Detached
			};

			struct
			{
				FILE* Input = nullptr;
				FILE* Output = nullptr;
				FILE* Errors = nullptr;
			} Streams;

			struct
			{
				unsigned short Attributes = 0;
				double Time = 0.0;
			} Cache;

		protected:
			std::recursive_mutex Session;
			Vector<ColorToken> BaseTokens;
			Vector<ColorToken> Tokens;
			Mode Status;
			bool Colors;

		public:
			Console() noexcept;
			virtual ~Console() noexcept override;
			void Hide();
			void Show();
			void Clear();
			void Attach();
			void Detach();
			void Allocate();
			void Deallocate();
			void Flush();
			void FlushWrite();
			void Trace(uint32_t MaxFrames = 32);
			void CaptureTime();
			void SetColoring(bool Enabled);
			void SetCursor(uint32_t X, uint32_t Y);
			void SetColorTokens(Vector<ColorToken>&& AdditionalTokens);
			void ColorBegin(StdColor Text, StdColor Background = StdColor::Black);
			void ColorEnd();
			void ColorPrint(StdColor BaseColor, const String& Buffer);
			void ColorPrintBuffer(StdColor BaseColor, const char* Buffer, size_t Size);
			void WriteBuffer(const char* Buffer);
			void WriteLine(const String& Line);
			void WriteChar(char Value);
			void Write(const String& Line);
			void fWriteLine(const char* Format, ...);
			void fWrite(const char* Format, ...);
			void sWriteLine(const String& Line);
			void sWrite(const String& Line);
			void sfWriteLine(const char* Format, ...);
			void sfWrite(const char* Format, ...);
			void GetSize(uint32_t* Width, uint32_t* Height);
			double GetCapturedTime() const;
			bool ReadLine(String& Data, size_t Size);
			String Read(size_t Size);
			char ReadChar();

		public:
			template <typename F>
			void Synced(F&& Callback)
			{
				UMutex<std::recursive_mutex> Unique(Session);
				Callback(this);
			}

		public:
			static bool IsAvailable();
		};

		class VI_OUT Timer final : public Reference<Timer>
		{
		public:
			typedef std::chrono::microseconds Units;

		public:
			struct Capture
			{
				const char* Name;
				Units Begin = Units(0);
				Units End = Units(0);
				Units Delta = Units(0);
				float Step = 0.0;
			};

		private:
			struct
			{
				Units Begin = Units(0);
				Units When = Units(0);
				Units Delta = Units(0);
				size_t Frame = 0;
			} Timing;

			struct
			{
				Units When = Units(0);
				Units Delta = Units(0);
				Units Sum = Units(0);
				size_t Frame = 0;
				bool InFrame = false;
			} Fixed;

		private:
			SingleQueue<Capture> Captures;
			Units MinDelta = Units(0);
			Units MaxDelta = Units(0);
			float FixedFrames = 0.0f;
			float MaxFrames = 0.0f;

		public:
			Timer() noexcept;
			~Timer() noexcept = default;
			void SetFixedFrames(float Value);
			void SetMaxFrames(float Value);
			void Reset();
			void Begin();
			void Finish();
			void Push(const char* Name = nullptr);
			bool PopIf(float GreaterThan, Capture* Out = nullptr);
			Capture Pop();
			size_t GetFrameIndex() const;
			size_t GetFixedFrameIndex() const;
			float GetMaxFrames() const;
			float GetMinStep() const;
			float GetFrames() const;
			float GetElapsed() const;
			float GetElapsedMills() const;
			float GetStep() const;
			float GetFixedStep() const;
			float GetFixedFrames() const;
			bool IsFixed() const;

		public:
			static float ToSeconds(const Units& Value);
			static float ToMills(const Units& Value);
			static Units ToUnits(float Value);
			static Units Clock();
		};

		class VI_OUT Stream : public Reference<Stream>
		{
		protected:
			String Path;
			size_t VirtualSize;

		public:
			Stream() noexcept;
			virtual ~Stream() noexcept = default;
			virtual Expected<void> Clear() = 0;
			virtual Expected<void> Open(const char* File, FileMode Mode) = 0;
			virtual Expected<void> Close() = 0;
			virtual Expected<void> Seek(FileSeek Mode, int64_t Offset) = 0;
			virtual Expected<void> Move(int64_t Offset) = 0;
			virtual Expected<void> Flush() = 0;
			virtual size_t ReadAny(const char* Format, ...) = 0;
			virtual size_t Read(char* Buffer, size_t Length) = 0;
			virtual size_t WriteAny(const char* Format, ...) = 0;
			virtual size_t Write(const char* Buffer, size_t Length) = 0;
			virtual size_t Tell() = 0;
			virtual socket_t GetFd() const = 0;
			virtual void* GetResource() const = 0;
			virtual bool IsSized() const = 0;
			void SetVirtualSize(size_t Size);
			size_t ReadAll(const std::function<void(char*, size_t)>& Callback);
			size_t GetVirtualSize() const;
			size_t GetSize();
			String& GetSource();
		};

		class VI_OUT FileStream : public Stream
		{
		protected:
			FILE* Resource;

		public:
			FileStream() noexcept;
			~FileStream() noexcept override;
			virtual Expected<void> Clear() override;
			virtual Expected<void> Open(const char* File, FileMode Mode) override;
			virtual Expected<void> Close() override;
			Expected<void> Seek(FileSeek Mode, int64_t Offset) override;
			Expected<void> Move(int64_t Offset) override;
			Expected<void> Flush() override;
			size_t ReadAny(const char* Format, ...) override;
			size_t Read(char* Buffer, size_t Length) override;
			size_t WriteAny(const char* Format, ...) override;
			size_t Write(const char* Buffer, size_t Length) override;
			size_t Tell() override;
			socket_t GetFd() const override;
			void* GetResource() const override;
			virtual bool IsSized() const override;
		};

		class VI_OUT GzStream final : public Stream
		{
		protected:
			void* Resource;

		public:
			GzStream() noexcept;
			~GzStream() noexcept override;
			Expected<void> Clear() override;
			Expected<void> Open(const char* File, FileMode Mode) override;
			Expected<void> Close() override;
			Expected<void> Seek(FileSeek Mode, int64_t Offset) override;
			Expected<void> Move(int64_t Offset) override;
			Expected<void> Flush() override;
			size_t ReadAny(const char* Format, ...) override;
			size_t Read(char* Buffer, size_t Length) override;
			size_t WriteAny(const char* Format, ...) override;
			size_t Write(const char* Buffer, size_t Length) override;
			size_t Tell() override;
			socket_t GetFd() const override;
			void* GetResource() const override;
			bool IsSized() const override;
		};

		class VI_OUT WebStream final : public Stream
		{
		protected:
			void* Resource;
			UnorderedMap<String, String> Headers;
			Vector<char> Chunk;
			size_t Offset;
			size_t Size;
			bool Async;

		public:
			WebStream(bool IsAsync) noexcept;
			WebStream(bool IsAsync, UnorderedMap<String, String>&& NewHeaders) noexcept;
			~WebStream() noexcept override;
			Expected<void> Clear() override;
			Expected<void> Open(const char* File, FileMode Mode) override;
			Expected<void> Close() override;
			Expected<void> Seek(FileSeek Mode, int64_t Offset) override;
			Expected<void> Move(int64_t Offset) override;
			Expected<void> Flush() override;
			size_t ReadAny(const char* Format, ...) override;
			size_t Read(char* Buffer, size_t Length) override;
			size_t WriteAny(const char* Format, ...) override;
			size_t Write(const char* Buffer, size_t Length) override;
			size_t Tell() override;
			socket_t GetFd() const override;
			void* GetResource() const override;
			bool IsSized() const override;
		};

		class VI_OUT ProcessStream final : public FileStream
		{
		private:
			int ExitCode;

		public:
			ProcessStream() noexcept;
			~ProcessStream() noexcept = default;
			Expected<void> Clear() override;
			Expected<void> Open(const char* File, FileMode Mode) override;
			Expected<void> Close() override;
			bool IsSized() const override;
			int GetExitCode() const;

		private:
			static FILE* OpenPipe(const char* File, const char* Mode);
			static int ClosePipe(void* Fd);
		};

		class VI_OUT FileTree final : public Reference<FileTree>
		{
		public:
			Vector<FileTree*> Directories;
			Vector<String> Files;
			String Path;

		public:
			FileTree(const String& Path) noexcept;
			~FileTree() noexcept;
			void Loop(const std::function<bool(const FileTree*)>& Callback) const;
			const FileTree* Find(const String& Path) const;
			size_t GetFiles() const;
		};

		class VI_OUT Costate final : public Reference<Costate>
		{
			friend Cocontext;

		private:
			UnorderedSet<Coroutine*> Cached;
			UnorderedSet<Coroutine*> Used;
			std::thread::id Thread;
			Coroutine* Current;
			Cocontext* Master;
			size_t Size;

		public:
			std::mutex* ExternalMutex;

		public:
			Costate(size_t StackSize = STACK_SIZE) noexcept;
			~Costate() noexcept;
			Costate(const Costate&) = delete;
			Costate(Costate&&) = delete;
			Costate& operator= (const Costate&) = delete;
			Costate& operator= (Costate&&) = delete;
			Coroutine* Pop(const TaskCallback& Procedure);
			Coroutine* Pop(TaskCallback&& Procedure);
			int Reuse(Coroutine* Routine, const TaskCallback& Procedure);
			int Reuse(Coroutine* Routine, TaskCallback&& Procedure);
			int Reuse(Coroutine* Routine);
			int Push(Coroutine* Routine);
			int Activate(Coroutine* Routine);
			int Deactivate(Coroutine* Routine);
			int Deactivate(Coroutine* Routine, const TaskCallback& AfterSuspend);
			int Deactivate(Coroutine* Routine, TaskCallback&& AfterSuspend);
			int Resume(Coroutine* Routine);
			int Dispatch();
			int Suspend();
			void Clear();
			bool HasActive() const;
			Coroutine* GetCurrent() const;
			size_t GetCount() const;

		private:
			int Swap(Coroutine* Routine);

		public:
			static Costate* Get();
			static Coroutine* GetCoroutine();
			static bool GetState(Costate** State, Coroutine** Routine);
			static bool IsCoroutine();

		public:
			static void VI_COCALL ExecutionEntry(VI_CODATA);
		};

		class VI_OUT Schema final : public Reference<Schema>
		{
		protected:
			Vector<Schema*>* Nodes;
			Schema* Parent;
			bool Saved;

		public:
			String Key;
			Variant Value;

		public:
			Schema(const Variant& Base) noexcept;
			Schema(Variant&& Base) noexcept;
			~Schema() noexcept;
			UnorderedMap<String, size_t> GetNames() const;
			Vector<Schema*> FindCollection(const String& Name, bool Deep = false) const;
			Vector<Schema*> FetchCollection(const String& Notation, bool Deep = false) const;
			Vector<Schema*> GetAttributes() const;
			Vector<Schema*>& GetChilds();
			Schema* Find(const String& Name, bool Deep = false) const;
			Schema* Fetch(const String& Notation, bool Deep = false) const;
			Variant FetchVar(const String& Key, bool Deep = false) const;
			Variant GetVar(size_t Index) const;
			Variant GetVar(const String& Key) const;
			Variant GetAttributeVar(const String& Key) const;
			Schema* GetParent() const;
			Schema* GetAttribute(const String& Key) const;
			Schema* Get(size_t Index) const;
			Schema* Get(const String& Key) const;
			Schema* Set(const String& Key);
			Schema* Set(const String& Key, const Variant& Value);
			Schema* Set(const String& Key, Variant&& Value);
			Schema* Set(const String& Key, Unique<Schema> Value);
			Schema* SetAttribute(const String& Key, const Variant& Value);
			Schema* SetAttribute(const String& Key, Variant&& Value);
			Schema* Push(const Variant& Value);
			Schema* Push(Variant&& Value);
			Schema* Push(Unique<Schema> Value);
			Schema* Pop(size_t Index);
			Schema* Pop(const String& Name);
			Unique<Schema> Copy() const;
			bool Rename(const String& Name, const String& NewName);
			bool Has(const String& Name) const;
			bool HasAttribute(const String& Name) const;
			bool IsEmpty() const;
			bool IsAttribute() const;
			bool IsSaved() const;
			size_t Size() const;
			String GetName() const;
			void Join(Schema* Other, bool AppendOnly);
			void Reserve(size_t Size);
			void Unlink();
			void Clear();
			void Save();

		protected:
			void Allocate();
			void Allocate(const Vector<Schema*>& Other);

		private:
			void Attach(Schema* Root);

		public:
			static void Transform(Schema* Value, const SchemaNameCallback& Callback);
			static void ConvertToXML(Schema* Value, const SchemaWriteCallback& Callback);
			static void ConvertToJSON(Schema* Value, const SchemaWriteCallback& Callback);
			static void ConvertToJSONB(Schema* Value, const SchemaWriteCallback& Callback);
			static String ToXML(Schema* Value);
			static String ToJSON(Schema* Value);
			static Vector<char> ToJSONB(Schema* Value);
			static Expects<Unique<Schema>, Exceptions::ParserException> ConvertFromXML(const char* Buffer);
			static Expects<Unique<Schema>, Exceptions::ParserException> ConvertFromJSON(const char* Buffer, size_t Size);
			static Expects<Unique<Schema>, Exceptions::ParserException> ConvertFromJSONB(const SchemaReadCallback& Callback);
			static Expects<Unique<Schema>, Exceptions::ParserException> FromXML(const String& Text);
			static Expects<Unique<Schema>, Exceptions::ParserException> FromJSON(const String& Text);
			static Expects<Unique<Schema>, Exceptions::ParserException> FromJSONB(const Vector<char>& Binary);

		private:
			static Expects<void, Exceptions::ParserException> ProcessConvertionFromJSONB(Schema* Current, UnorderedMap<size_t, String>* Map, const SchemaReadCallback& Callback);
			static void ProcessConvertionFromXML(void* Base, Schema* Current);
			static void ProcessConvertionFromJSON(void* Base, Schema* Current);
			static void ProcessConvertionToJSONB(Schema* Current, UnorderedMap<String, size_t>* Map, const SchemaWriteCallback& Callback);
			static void GenerateNamingTable(const Schema* Current, UnorderedMap<String, size_t>* Map, size_t& Index);
		};

		class VI_OUT_TS Schedule final : public Singleton<Schedule>
		{
		public:
			enum class ThreadTask
			{
				Spawn,
				EnqueueTimer,
				EnqueueCoroutine,
				EnqueueTask,
				ConsumeCoroutine,
				ProcessTimer,
				ProcessCoroutine,
				ProcessTask,
				Awake,
				Sleep,
				Despawn
			};

			struct VI_OUT ThreadPtr
			{
				std::condition_variable Notify;
				std::mutex Update;
				std::thread Handle;
				std::thread::id Id;
				Allocators::LinearAllocator Allocator;
				Difficulty Type;
				size_t GlobalIndex;
				size_t LocalIndex;
				bool Daemon;

				ThreadPtr(Difficulty NewType, size_t PreallocatedSize, size_t NewGlobalIndex, size_t NewLocalIndex, bool IsDaemon) : Allocator(PreallocatedSize), Type(NewType), GlobalIndex(NewGlobalIndex), LocalIndex(NewLocalIndex), Daemon(IsDaemon)
				{
				}
				~ThreadPtr() = default;
			};

			struct VI_OUT ThreadMessage
			{
				const ThreadPtr* Thread;
				ThreadTask State;
				size_t Tasks;

				ThreadMessage(const ThreadPtr* NewThread, ThreadTask NewState, size_t NewTasks) : Thread(NewThread), State(NewState), Tasks(NewTasks)
				{
				}
			};

			struct VI_OUT Desc
			{
				std::chrono::milliseconds Timeout = std::chrono::milliseconds(2000);
				size_t Threads[(size_t)Difficulty::Count] = { 1, 1, 1, 1 };
				size_t PreallocatedSize = 0;
				size_t StackSize = STACK_SIZE;
				size_t MaxCoroutines = 16;
				ActivityCallback Ping = nullptr;
				bool Parallel = true;

				Desc();
				void SetThreads(size_t Cores);
			};

			typedef std::function<void(const ThreadMessage&)> ThreadDebugCallback;

		private:
			struct
			{
				Vector<TaskCallback> Events;
				TaskCallback Tasks[EVENTS_SIZE];
				Costate* State = nullptr;
			} Dispatcher;

		private:
			ConcurrentQueue* Queues[(size_t)Difficulty::Count];
			Vector<ThreadPtr*> Threads[(size_t)Difficulty::Count];
			std::atomic<TaskId> Generation;
			std::mutex Exclusive;
			ThreadDebugCallback Debug;
			Desc Policy;
			bool Enqueue;
			bool Terminate;
			bool Active;
			bool Immediate;

		public:
			Schedule() noexcept;
			virtual ~Schedule() noexcept override;
			TaskId SetInterval(uint64_t Milliseconds, const TaskCallback& Callback, Difficulty Type = Difficulty::Light);
			TaskId SetInterval(uint64_t Milliseconds, TaskCallback&& Callback, Difficulty Type = Difficulty::Light);
			TaskId SetTimeout(uint64_t Milliseconds, const TaskCallback& Callback, Difficulty Type = Difficulty::Light);
			TaskId SetTimeout(uint64_t Milliseconds, TaskCallback&& Callback, Difficulty Type = Difficulty::Light);
			bool SetTask(const TaskCallback& Callback, Difficulty Type = Difficulty::Heavy);
			bool SetTask(TaskCallback&& Callback, Difficulty Type = Difficulty::Heavy);
			bool SetCoroutine(const TaskCallback& Callback);
			bool SetCoroutine(TaskCallback&& Callback);
			bool SetDebugCallback(const ThreadDebugCallback& Callback);
			bool SetImmediate(bool Enabled);
			bool ClearTimeout(TaskId WorkId);
			bool Start(const Desc& NewPolicy);
			bool Stop();
			bool Wakeup();
			bool Dispatch();
			bool IsActive() const;
			bool CanEnqueue() const;
			bool HasTasks(Difficulty Type) const;
			bool HasAnyTasks() const;
			size_t GetThreadGlobalIndex();
			size_t GetThreadLocalIndex();
			size_t GetTotalThreads() const;
			size_t GetThreads(Difficulty Type) const;
			const ThreadPtr* GetThread() const;
			const Desc& GetPolicy() const;

		private:
			const ThreadPtr* InitializeThread(ThreadPtr* Source, bool Update) const;
			bool PostDebug(ThreadTask State, size_t Tasks);
			bool ProcessTick(Difficulty Type);
			bool ProcessLoop(Difficulty Type, ThreadPtr* Thread);
			bool ThreadActive(ThreadPtr* Thread);
			bool ChunkCleanup();
			bool PushThread(Difficulty Type, size_t GlobalIndex, size_t LocalIndex, bool IsDaemon);
			bool PopThread(ThreadPtr* Thread);
			std::chrono::microseconds GetTimeout(std::chrono::microseconds Clock);
			TaskId GetTaskId();

		public:
			static std::chrono::microseconds GetClock();
			static bool IsAvailable();
		};

		template <>
		class UAlloc<Schedule>
		{
		private:
			Schedule::ThreadPtr* Thread;

		public:
			UAlloc() noexcept : Thread((Schedule::ThreadPtr*)Schedule::Get()->GetThread())
			{
				VI_PANIC(!Thread, "this thread is not a scheduler thread");
				Memory::SetLocalAllocator(&Thread->Allocator);
			}
			UAlloc(const UAlloc& Other) noexcept = delete;
			UAlloc(UAlloc&& Other) noexcept = delete;
			~UAlloc()
			{
				Memory::SetLocalAllocator(nullptr);
				Thread->Allocator.Reset();
			}
			UAlloc& operator= (const UAlloc& Other) noexcept = delete;
			UAlloc& operator= (UAlloc&& Other) noexcept = delete;
		};

		template <typename T>
		class Pool
		{
			static_assert(std::is_pointer<T>::value, "pool can be used only for pointer types");

		public:
			typedef T* Iterator;

		protected:
			size_t Count, Volume;
			T* Data;

		public:
			Pool() noexcept : Count(0), Volume(0), Data(nullptr)
			{
			}
			Pool(const Pool<T>& Ref) noexcept : Count(0), Volume(0), Data(nullptr)
			{
				if (Ref.Data != nullptr)
					Copy(Ref);
			}
			Pool(Pool<T>&& Ref) noexcept : Count(Ref.Count), Volume(Ref.Volume), Data(Ref.Data)
			{
				Ref.Count = 0;
				Ref.Volume = 0;
				Ref.Data = nullptr;
			}
			~Pool() noexcept
			{
				Release();
			}
			void Release()
			{
				VI_FREE(Data);
				Data = nullptr;
				Count = 0;
				Volume = 0;
			}
			void Reserve(size_t Capacity)
			{
				if (Capacity <= Volume)
					return;

				size_t Size = Capacity * sizeof(T);
				T* NewData = VI_MALLOC(T, Size);
				memset(NewData, 0, Size);
				
				if (Data != nullptr)
				{
					memcpy(NewData, Data, Count * sizeof(T));
					VI_FREE(Data);
				}

				Volume = Capacity;
				Data = NewData;
			}
			void Resize(size_t NewSize)
			{
				if (NewSize > Volume)
					Reserve(IncreaseCapacity(NewSize));

				Count = NewSize;
			}
			void Copy(const Pool<T>& Ref)
			{
				if (!Ref.Data)
					return;

				if (!Data || Ref.Count > Count)
				{
					VI_FREE(Data);
					Data = VI_MALLOC(T, Ref.Volume * sizeof(T));
					memset(Data, 0, Ref.Volume * sizeof(T));
				}

				memcpy(Data, Ref.Data, (size_t)(Ref.Count * sizeof(T)));
				Count = Ref.Count;
				Volume = Ref.Volume;
			}
			void Clear()
			{
				Count = 0;
			}
			Iterator Add(const T& Ref)
			{
				VI_ASSERT(Count < Volume, "pool capacity overflow");
				Data[Count++] = Ref;
				return End() - 1;
			}
			Iterator AddUnbounded(const T& Ref)
			{
				if (Count + 1 >= Volume)
					Reserve(IncreaseCapacity(Count + 1));

				return Add(Ref);
			}
			Iterator AddIfNotExists(const T& Ref)
			{
				Iterator It = Find(Ref);
				if (It != End())
					return It;

				return Add(Ref);
			}
			Iterator RemoveAt(Iterator It)
			{
				VI_ASSERT(Count > 0, "pool is empty");
				VI_ASSERT((size_t)(It - Data) >= 0 && (size_t)(It - Data) < Count, "iterator ranges out of pool");

				Count--;
				Data[It - Data] = Data[Count];
				return It;
			}
			Iterator Remove(const T& Value)
			{
				Iterator It = Find(Value);
				if (It != End())
					RemoveAt(It);

				return It;
			}
			Iterator At(size_t Index) const
			{
				VI_ASSERT(Index < Count, "index ranges out of pool");
				return Data + Index;
			}
			Iterator Find(const T& Ref)
			{
				auto End = Data + Count;
				for (auto It = Data; It != End; ++It)
				{
					if (*It == Ref)
						return It;
				}

				return End;
			}
			Iterator Begin() const
			{
				return Data;
			}
			Iterator End() const
			{
				return Data + Count;
			}
			Iterator begin() const
			{
				return Data;
			}
			Iterator end() const
			{
				return Data + Count;
			}
			size_t Size() const
			{
				return Count;
			}
			size_t Capacity() const
			{
				return Volume;
			}
			bool Empty() const
			{
				return !Count;
			}
			T* Get() const
			{
				return Data;
			}
			T& Front() const
			{
				return *Begin();
			}
			T& Back() const
			{
				return *(End() - 1);
			}
			T& operator [](size_t Index) const
			{
				return *(Data + Index);
			}
			T& operator [](size_t Index)
			{
				return *(Data + Index);
			}
			Pool<T>& operator =(const Pool<T>& Ref) noexcept
			{
				if (this == &Ref)
					return *this;

				Copy(Ref);
				return *this;
			}
			Pool<T>& operator =(Pool<T>&& Ref) noexcept
			{
				if (this == &Ref)
					return *this;

				Release();
				Count = Ref.Count;
				Volume = Ref.Volume;
				Data = Ref.Data;
				Ref.Count = 0;
				Ref.Volume = 0;
				Ref.Data = nullptr;
				return *this;
			}
			bool operator ==(const Pool<T>& Raw)
			{
				return Compare(Raw) == 0;
			}
			bool operator !=(const Pool<T>& Raw)
			{
				return Compare(Raw) != 0;
			}
			bool operator <(const Pool<T>& Raw)
			{
				return Compare(Raw) < 0;
			}
			bool operator <=(const Pool<T>& Raw)
			{
				return Compare(Raw) <= 0;
			}
			bool operator >(const Pool<T>& Raw)
			{
				return Compare(Raw) > 0;
			}
			bool operator >=(const Pool<T>& Raw)
			{
				return Compare(Raw) >= 0;
			}

		private:
			size_t IncreaseCapacity(size_t NewSize)
			{
				size_t Alpha = Volume > 4 ? (Volume + Volume / 2) : 8;
				return Alpha > NewSize ? Alpha : NewSize;
			}
		};

		struct VI_OUT_TS ParallelExecutor
		{
			inline void operator()(TaskCallback&& Callback)
			{
				if (Schedule::IsAvailable())
					Schedule::Get()->SetTask(std::move(Callback), Difficulty::Light);
				else
					Callback();
			}
		};

		template <typename T>
		class DeferredStorage
		{
		public:
			TaskCallback Event;
			char Value[sizeof(T)];
			std::atomic<uint32_t> Count;
			std::atomic<Deferred> Code;
			std::mutex Update;

		public:
			DeferredStorage() noexcept : Count(1), Code(Deferred::Pending)
			{
			}
			DeferredStorage(const T& NewValue) noexcept : Count(1), Code(Deferred::Ready)
			{
				new(Value) T(NewValue);
			}
			DeferredStorage(T&& NewValue) noexcept : Count(1), Code(Deferred::Ready)
			{
				new(Value) T(std::move(NewValue));
			}
			~DeferredStorage()
			{
				if (Code == Deferred::Ready)
					((T*)Value)->~T();
			}
			void Emplace(const T& NewValue)
			{
				VI_ASSERT(Code != Deferred::Ready, "emplacing to already initialized memory is not desired");
				new(Value) T(NewValue);
			}
			void Emplace(T&& NewValue)
			{
				VI_ASSERT(Code != Deferred::Ready, "emplacing to already initialized memory is not desired");
				new(Value) T(std::move(NewValue));
			}
			T& Unwrap()
			{
				VI_ASSERT(Code == Deferred::Ready, "unwrapping uninitialized memory will result in undefined behaviour");
				return *(T*)Value;
			}
		};

		template <>
		class DeferredStorage<void>
		{
		public:
			TaskCallback Event;
			std::mutex Update;
			std::atomic<uint32_t> Count;
			std::atomic<Deferred> Code;

		public:
			DeferredStorage() noexcept : Count(1), Code(Deferred::Pending)
			{
			}
		};

		template <typename T, typename Executor>
		class BasicPromise
		{
		public:
			struct awaitable
			{
#ifdef VI_CXX20
				BasicPromise Value;

				explicit awaitable(const BasicPromise& NewValue) : Value(NewValue)
				{
				}
				explicit awaitable(BasicPromise&& NewValue) : Value(std::move(NewValue))
				{
				}
				awaitable(const awaitable&) = default;
				awaitable(awaitable&&) = default;
				bool await_ready() const noexcept
				{
					return !Value.IsPending();
				}
				T&& await_resume() noexcept
				{
					return Value.Get();
				}
				void await_suspend(std::coroutine_handle<> Handle)
				{
					Value.When([Handle](T&&)
					{
						Handle.resume();
					});
				}
#endif
			};

			struct promise_type
			{
#ifdef VI_CXX20
				BasicPromise Value;
#ifndef NDEBUG
				std::chrono::microseconds Time;
#endif
				promise_type()
				{
#ifndef NDEBUG
					Time = Schedule::GetClock();
					VI_WATCH((void*)&Value, "coroutine20-frame");
#endif
				}
				std::suspend_never initial_suspend() const noexcept
				{
					return {};
				}
				std::suspend_never final_suspend() const noexcept
				{
					return {};
				}
				BasicPromise get_return_object()
				{
#ifndef NDEBUG
					int64_t Diff = (Schedule::GetClock() - Time).count();
					if (Diff > (int64_t)Timings::Hangup * 1000)
						VI_WARN("[stall] async operation took %" PRIu64 " ms (%" PRIu64 " us)\t\nexpected: %" PRIu64 " ms at most", Diff / 1000, Diff, (uint64_t)Timings::Hangup);
					VI_UNWATCH((void*)&Value);
#endif
					return Value;
				}
				void return_value(const BasicPromise& NewValue)
				{
					Value.Set(NewValue);
				}
				void return_value(const T& NewValue)
				{
					Value.Set(NewValue);
				}
				void return_value(T&& NewValue)
				{
					Value.Set(std::move(NewValue));
				}
				void unhandled_exception()
				{
				}
				void* operator new(size_t Size) noexcept
				{
					return VI_MALLOC(promise_type, Size);
				}
				void operator delete(void* Ptr) noexcept
				{
					VI_FREE(Ptr);
				}
				static BasicPromise get_return_object_on_allocation_failure()
				{
					return BasicPromise::Null();
				}
#endif
			};

		public:
			typedef DeferredStorage<T> Status;
			typedef T Type;

		private:
			template <typename F>
			struct Unwrap
			{
				typedef typename std::remove_reference<F>::type type;
			};

			template <typename F>
			struct Unwrap<BasicPromise<F, Executor>>
			{
				typedef typename std::remove_reference<F>::type type;
			};

		private:
			Status* Data;

		public:
			BasicPromise() noexcept : Data(VI_NEW(Status))
			{
			}
			BasicPromise(const T& Value) noexcept : Data(VI_NEW(Status, Value))
			{
			}
			BasicPromise(T&& Value) noexcept : Data(VI_NEW(Status, std::move(Value)))
			{
			}
			BasicPromise(const BasicPromise& Other) : Data(Other.Data)
			{
				AddRef();
			}
			BasicPromise(BasicPromise&& Other) noexcept : Data(Other.Data)
			{
				Other.Data = nullptr;
			}
			~BasicPromise() noexcept
			{
				Release(Data);
			}
			BasicPromise& operator= (const BasicPromise& Other) = delete;
			BasicPromise& operator= (BasicPromise&& Other) noexcept
			{
				if (&Other == this)
					return *this;

				Release(Data);
				Data = Other.Data;
				Other.Data = nullptr;
				return *this;
			}
			void Set(const T& Other)
			{
				VI_ASSERT(Data != nullptr && Data->Code != Deferred::Ready, "async should be pending");
				UMutex<std::mutex> Unique(Data->Update);
				bool Async = (Data->Code != Deferred::Waiting);
				Data->Emplace(Other);
				Data->Code = Deferred::Ready;
				Execute(Data, Async);
			}
			void Set(T&& Other)
			{
				VI_ASSERT(Data != nullptr && Data->Code != Deferred::Ready, "async should be pending");
				UMutex<std::mutex> Unique(Data->Update);
				bool Async = (Data->Code != Deferred::Waiting);
				Data->Emplace(std::move(Other));
				Data->Code = Deferred::Ready;
				Execute(Data, Async);
			}
			void Set(const BasicPromise& Other)
			{
				VI_ASSERT(Data != nullptr && Data->Code != Deferred::Ready, "async should be pending");
				Status* Copy = AddRef();
				Other.When([Copy](T&& Value) mutable
				{
					{
						UMutex<std::mutex> Unique(Copy->Update);
						bool Async = (Copy->Code != Deferred::Waiting);
						Copy->Emplace(std::move(Value));
						Copy->Code = Deferred::Ready;
						Execute(Copy, Async);
					}
					Release(Copy);
				});
			}
			void When(std::function<void(T&&)>&& Callback) const noexcept
			{
				VI_ASSERT(Data != nullptr && Callback, "callback should be set");
				if (!IsPending())
					return Callback(std::move(Data->Unwrap()));

				Status* Copy = AddRef();
				Store([Copy, Callback = std::move(Callback)]() mutable
				{
					Callback(std::move(Copy->Unwrap()));
					Release(Copy);
				});
			}
			void Wait()
			{
				if (!IsPending())
					return;

				Status* Copy;
				{
					std::unique_lock<std::mutex> Lock(Data->Update);
					if (Data->Code == Deferred::Ready)
						return;

					std::condition_variable Ready;
					Copy = AddRef();
					Copy->Code = Deferred::Waiting;
					Copy->Event = [&Ready]()
					{
						Ready.notify_all();
					};
					Ready.wait(Lock, [this]()
					{
						return !IsPending();
					});
				}
				Release(Copy);
			}
			T&& Get() noexcept
			{
				Wait();
				return Load();
			}
			Deferred GetStatus() const noexcept
			{
				return Data ? Data->Code.load() : Deferred::Ready;
			}
			bool IsPending() const noexcept
			{
				return Data ? Data->Code != Deferred::Ready : false;
			}
			template <typename R>
			BasicPromise<R, Executor> Then(std::function<void(BasicPromise<R, Executor>&, T&&)>&& Callback) const noexcept
			{
				VI_ASSERT(Data != nullptr && Callback, "async should be pending");
				BasicPromise<R, Executor> Result; Status* Copy = AddRef();
				Store([Copy, Result, Callback = std::move(Callback)]() mutable
				{
					Callback(Result, std::move(Copy->Unwrap()));
					Release(Copy);
				});

				return Result;
			}
			template <typename R>
			BasicPromise<typename Unwrap<R>::type, Executor> Then(std::function<R(T&&)>&& Callback) const noexcept
			{
				VI_ASSERT(Data != nullptr && Callback, "async should be pending");
				BasicPromise<typename Unwrap<R>::type, Executor> Result; Status* Copy = AddRef();
				Store([Copy, Result, Callback = std::move(Callback)]() mutable
				{
					Result.Set(std::move(Callback(std::move(Copy->Unwrap()))));
					Release(Copy);
				});

				return Result;
			}

		private:
			BasicPromise(bool Unused1, bool Unused2) noexcept : Data(nullptr)
			{
			}
			Status* AddRef() const
			{
				if (Data != nullptr)
					++Data->Count;
				return Data;
			}
			T&& Load()
			{
				if (!Data)
					Data = VI_NEW(Status);

				return std::move(Data->Unwrap());
			}
			void Store(TaskCallback&& Callback) const noexcept
			{
				UMutex<std::mutex> Unique(Data->Update);
				Data->Event = std::move(Callback);
				if (Data->Code == Deferred::Ready)
					Execute(Data, false);
			}

		public:
			static BasicPromise Null() noexcept
			{
				return BasicPromise(false, false);
			}

		private:
			static void Execute(Status* State, bool Async) noexcept
			{
				if (!State->Event)
					return;

				if (Async)
					Executor()(std::move(State->Event));
				else
					State->Event();
			}
			static void Release(Status* State) noexcept
			{
				if (State != nullptr && !--State->Count)
					VI_DELETE(Status, State);
			}
		};

		template <typename Executor>
		class BasicPromise<void, Executor>
		{
		public:
			struct awaitable
			{
#ifdef VI_CXX20
				BasicPromise Value;

				explicit awaitable(const BasicPromise& NewValue) : Value(NewValue)
				{
				}
				explicit awaitable(BasicPromise&& NewValue) : Value(std::move(NewValue))
				{
				}
				awaitable(const awaitable&) = default;
				awaitable(awaitable&&) = default;
				bool await_ready() const noexcept
				{
					return !Value.IsPending();
				}
				void await_resume() noexcept
				{
				}
				void await_suspend(std::coroutine_handle<> Handle)
				{
					Value.When([Handle]()
					{
						Handle.resume();
					});
				}
#endif
			};

			struct promise_type
			{
#ifdef VI_CXX20
				BasicPromise Value;
#ifndef NDEBUG
				std::chrono::microseconds Time;
#endif
				promise_type()
				{
#ifndef NDEBUG
					Time = Schedule::GetClock();
					VI_WATCH((void*)&Value, "coroutine20-frame");
#endif
				}
				std::suspend_never initial_suspend() const noexcept
				{
					return {};
				}
				std::suspend_never final_suspend() const noexcept
				{
					return {};
				}
				BasicPromise get_return_object()
				{
#ifndef NDEBUG
					int64_t Diff = (Schedule::GetClock() - Time).count();
					if (Diff > (int64_t)Timings::Hangup * 1000)
						VI_WARN("[stall] async operation took %" PRIu64 " ms (%" PRIu64 " us)\t\nexpected: %" PRIu64 " ms at most", Diff / 1000, Diff, (uint64_t)Timings::Hangup);
					VI_UNWATCH((void*)&Value);
#endif
					return Value;
				}
				void return_void()
				{
					Value.Set();
				}
				void unhandled_exception()
				{
				}
				void* operator new(size_t Size) noexcept
				{
					return VI_MALLOC(promise_type, Size);
				}
				void operator delete(void* Ptr) noexcept
				{
					VI_FREE(Ptr);
				}
				static BasicPromise get_return_object_on_allocation_failure()
				{
					return BasicPromise::Null();
				}
#endif
			};

		public:
			typedef DeferredStorage<void> Status;
			typedef void Type;

		private:
			template <typename F>
			struct Unwrap
			{
				typedef F type;
			};

			template <typename F>
			struct Unwrap<BasicPromise<F, Executor>>
			{
				typedef F type;
			};

		private:
			Status* Data;

		public:
			BasicPromise() noexcept : Data(VI_NEW(Status))
			{
			}
			BasicPromise(const BasicPromise& Other) noexcept : Data(Other.Data)
			{
				AddRef();
			}
			BasicPromise(BasicPromise&& Other) noexcept : Data(Other.Data)
			{
				Other.Data = nullptr;
			}
			~BasicPromise() noexcept
			{
				Release(Data);
			}
			BasicPromise& operator= (const BasicPromise& Other) = delete;
			BasicPromise& operator= (BasicPromise&& Other) noexcept
			{
				if (&Other == this)
					return *this;

				Release(Data);
				Data = Other.Data;
				Other.Data = nullptr;
				return *this;
			}
			void Set()
			{
				VI_ASSERT(Data != nullptr && Data->Code != Deferred::Ready, "async should be pending");
				UMutex<std::mutex> Unique(Data->Update);
				bool Async = (Data->Code != Deferred::Waiting);
				Data->Code = Deferred::Ready;
				Execute(Data, Async);
			}
			void Set(const BasicPromise& Other)
			{
				VI_ASSERT(Data != nullptr && Data->Code != Deferred::Ready, "async should be pending");
				Status* Copy = AddRef();
				Other.When([Copy]() mutable
				{
					{
						UMutex<std::mutex> Unique(Copy->Update);
						bool Async = (Copy->Code != Deferred::Waiting);
						Copy->Code = Deferred::Ready;
						Execute(Copy, Async);
					}
					Release(Copy);
				});
			}
			void When(std::function<void()>&& Callback) const noexcept
			{
				VI_ASSERT(Callback, "callback should be set");
				if (!IsPending())
					return Callback();

				Status* Copy = AddRef();
				Store([Copy, Callback = std::move(Callback)]() mutable
				{
					Callback();
					Release(Copy);
				});
			}
			void Wait()
			{
				if (!IsPending())
					return;

				Status* Copy;
				{
					std::unique_lock<std::mutex> Lock(Data->Update);
					if (Data->Code == Deferred::Ready)
						return;

					std::condition_variable Ready;
					Copy = AddRef();
					Copy->Code = Deferred::Waiting;
					Copy->Event = [&Ready]()
					{
						Ready.notify_all();
					};
					Ready.wait(Lock, [this]()
					{
						return !IsPending();
					});
				}
				Release(Copy);
			}
			void Get() noexcept
			{
				Wait();
			}
			Deferred GetStatus() const noexcept
			{
				return Data ? Data->Code.load() : Deferred::Ready;
			}
			bool IsPending() const noexcept
			{
				return Data ? Data->Code != Deferred::Ready : false;
			}
			template <typename R>
			BasicPromise<R, Executor> Then(std::function<void(BasicPromise<R, Executor>&)>&& Callback) const noexcept
			{
				VI_ASSERT(Callback, "callback should be set");
				BasicPromise<R, Executor> Result;
				if (!IsPending())
				{
					Callback(Result);
					return Result;
				}

				Status* Copy = AddRef();
				Store([Copy, Result, Callback = std::move(Callback)]() mutable
				{
					Callback(Result);
					Release(Copy);
				});

				return Result;
			}
			template <typename R>
			BasicPromise<typename Unwrap<R>::type, Executor> Then(std::function<R()>&& Callback) const noexcept
			{
				VI_ASSERT(Callback, "callback should be set");
				BasicPromise<typename Unwrap<R>::type, Executor> Result;
				if (!IsPending())
				{
					Callback();
					Result.Set();
					return Result;
				}
				
				Status* Copy = AddRef();
				Store([Copy, Result, Callback = std::move(Callback)]() mutable
				{
					Callback();
					Result.Set();
					Release(Copy);
				});

				return Result;
			}

		private:
			BasicPromise(Status* Context) noexcept : Data(Context)
			{
				AddRef();
			}
			Status* AddRef() const
			{
				if (Data != nullptr)
					++Data->Count;
				return Data;
			}
			void Load() noexcept
			{
				if (!Data)
					Data = VI_NEW(Status);
			}
			void Store(TaskCallback&& Callback) const noexcept
			{
				UMutex<std::mutex> Unique(Data->Update);
				Data->Event = std::move(Callback);
				if (Data->Code == Deferred::Ready)
					Execute(Data, false);
			}

		public:
			static BasicPromise Null() noexcept
			{
				return BasicPromise((Status*)nullptr);
			}

		private:
			static void Execute(Status* State, bool Async) noexcept
			{
				if (!State->Event)
					return;

				if (Async)
					Executor()(std::move(State->Event));
				else
					State->Event();
			}
			static void Release(Status* State) noexcept
			{
				if (State != nullptr && !--State->Count)
					VI_DELETE(Status, State);
			}
		};

		template <typename T, typename Executor = ParallelExecutor>
		using Promise = BasicPromise<T, Executor>;

		template <typename T, typename E, typename Executor = ParallelExecutor>
		using ExpectsPromise = BasicPromise<Expects<T, E>, Executor>;

		template <typename T, typename Executor = ParallelExecutor>
		using ExpectedPromise = BasicPromise<Expected<T>, Executor>;

		inline bool Cosuspend() noexcept
		{
			VI_ASSERT(Costate::Get() != nullptr, "cannot call suspend outside coroutine");
			return Costate::Get()->Suspend();
		}
		template <typename T, typename Executor = ParallelExecutor>
		inline Promise<T> Cotask(std::function<T()>&& Callback, Difficulty Type = Difficulty::Heavy) noexcept
		{
			VI_ASSERT(Callback, "callback should not be empty");

			Promise<T> Result;
			Schedule::Get()->SetTask([Result, Callback = std::move(Callback)]() mutable
			{
				Result.Set(std::move(Callback()));
			}, Type);

			return Result;
		}
#ifdef VI_CXX20
		template <typename T>
		struct PromiseContext
		{
			std::function<Promise<T>()> Callback;

			PromiseContext(std::function<Promise<T>()>&& NewCallback) : Callback(std::move(NewCallback))
			{
			}
			~PromiseContext() = default;
			PromiseContext(const PromiseContext& Other) = delete;
			PromiseContext(PromiseContext&& Other) = delete;
			PromiseContext& operator= (const PromiseContext& Other) = delete;
			PromiseContext& operator= (PromiseContext&& Other) = delete;
		};

		template <typename T, typename Executor = ParallelExecutor>
		auto operator co_await(BasicPromise<T, Executor>&& Value) noexcept
		{
			using Awaitable = typename BasicPromise<T, Executor>::awaitable;
			return Awaitable(std::move(Value));
		}
		template <typename T, typename Executor = ParallelExecutor>
		auto operator co_await(const BasicPromise<T, Executor>& Value) noexcept
		{
			using Awaitable = typename BasicPromise<T, Executor>::awaitable;
			return Awaitable(Value);
		}
		template <typename T>
		void Coforward1(Promise<T> Value, PromiseContext<T>* Context)
		{
			Promise<T> Wrapper = Context->Callback();
			auto Deleter = [Context](T&& Result) -> T&&
			{
				VI_DELETE(PromiseContext, Context);
				return std::move(Result);
			};
			Value.Set(Wrapper.template Then<T>(Deleter));
		}
		template <typename T>
		Promise<T> Coforward2(Promise<T>&& Value, PromiseContext<T>* Context)
		{
			auto Deleter = [Context](T&& Result) -> T&&
			{
				VI_DELETE(PromiseContext, Context);
				return std::move(Result);
			};
			return Value.template Then<T>(Deleter);
		}
		template <typename T>
		inline Promise<T> Coasync(std::function<Promise<T>()>&& Callback, bool AlwaysEnqueue = false) noexcept
		{
			VI_ASSERT(Callback != nullptr, "callback should be set");
			PromiseContext<T>* Context = VI_NEW(PromiseContext<T>, std::move(Callback));
			if (AlwaysEnqueue)
			{
				Promise<T> Value;
				Schedule::Get()->SetTask(std::bind(&Coforward1<T>, Value, Context), Difficulty::Light);
				return Value;
			}
			else
			{
				Promise<T> Value = Context->Callback();
				return Coforward2<T>(std::move(Value), Context);
			}
		}
		template <>
		inline Promise<void> Coasync(std::function<Promise<void>()>&& Callback, bool AlwaysEnqueue) noexcept
		{
			VI_ASSERT(Callback != nullptr, "callback should be set");
			PromiseContext<void>* Context = VI_NEW(PromiseContext<void>, std::move(Callback));
			if (AlwaysEnqueue)
			{
				Promise<void> Value;
				Schedule::Get()->SetTask([Value, Context]() mutable
				{
					Promise<void> Wrapper = Context->Callback();
					Value.Set(Wrapper.Then<void>([Context]()
					{
						VI_DELETE(PromiseContext, Context);
					}));
				}, Difficulty::Light);

				return Value;
			}
			else
			{
				Promise<void> Value = Context->Callback();
				return Value.Then<void>([Context]()
				{
					VI_DELETE(PromiseContext, Context);
				});
			}
		}
#else
		template <typename T>
		inline T&& Coawait(Promise<T>&& Future, const char* Function = nullptr, const char* Expression = nullptr) noexcept
		{
			if (!Future.IsPending())
				return Future.Get();

			Costate* State; Coroutine* Base;
			Costate::GetState(&State, &Base);
			VI_ASSERT(State != nullptr && Base != nullptr, "cannot call await outside coroutine");
#ifndef NDEBUG
			std::chrono::microseconds Time = Schedule::GetClock();
			if (Function != nullptr && Expression != nullptr)
				VI_WATCH_AT((void*)&Future, Function, Expression);
#endif
			State->Deactivate(Base, [&Future, &State, &Base]()
			{
				Future.When([&State, &Base](T&&)
				{
					State->Activate(Base);
				});
			});
#ifndef NDEBUG
			if (Function != nullptr && Expression != nullptr)
			{
				int64_t Diff = (Schedule::GetClock() - Time).count();
				if (Diff > (int64_t)Timings::Hangup * 1000)
					VI_WARN("[stall] %s(): \"%s\" operation took %" PRIu64 " ms (%" PRIu64 " us) out of % " PRIu64 "ms budget", Function, Expression, Diff / 1000, Diff, (uint64_t)Timings::Hangup);
				VI_UNWATCH((void*)&Future);
			}
#endif
			return Future.Get();
		}
		inline void Coawait(Promise<void>&& Future, const char* Function = nullptr, const char* Expression = nullptr) noexcept
		{
			if (!Future.IsPending())
				return Future.Get();

			Costate* State; Coroutine* Base;
			Costate::GetState(&State, &Base);
			VI_ASSERT(State != nullptr && Base != nullptr, "cannot call await outside coroutine");
#ifndef NDEBUG
			std::chrono::microseconds Time = Schedule::GetClock();
			if (Function != nullptr && Expression != nullptr)
				VI_WATCH_AT((void*)&Future, Function, Expression);
#endif
			State->Deactivate(Base, [&Future, &State, &Base]()
			{
				Future.When([&State, &Base]()
				{
					State->Activate(Base);
				});
			});
#ifndef NDEBUG
			if (Function != nullptr && Expression != nullptr)
			{
				int64_t Diff = (Schedule::GetClock() - Time).count();
				if (Diff > (int64_t)Timings::Hangup * 1000)
					VI_WARN("[stall] %s(): \"%s\" operation took %" PRIu64 " ms (%" PRIu64 " us) out of % " PRIu64 "ms budget", Function, Expression, Diff / 1000, Diff, (uint64_t)Timings::Hangup);
				VI_UNWATCH((void*)&Future);
			}
#endif
			return Future.Get();
		}
		template <typename T>
		inline Promise<T> Coasync(std::function<Promise<T>()>&& Callback, bool AlwaysEnqueue = false) noexcept
		{
			VI_ASSERT(Callback, "callback should not be empty");
			if (!AlwaysEnqueue && Costate::IsCoroutine())
				return Callback();

			Promise<T> Result;
			Schedule::Get()->SetCoroutine([Result, Callback = std::move(Callback)]() mutable
			{
				auto Task = Callback();
				Result.Set(VI_AWAIT(std::move(Task)));
			});

			return Result;
		}
		template <>
		inline Promise<void> Coasync(std::function<Promise<void>()>&& Callback, bool AlwaysEnqueue) noexcept
		{
			VI_ASSERT(Callback, "callback should not be empty");
			if (!AlwaysEnqueue && Costate::IsCoroutine())
				return Callback();

			Promise<void> Result;
			Schedule::Get()->SetCoroutine([Result, Callback = std::move(Callback)]() mutable
			{
				Callback();
				Result.Set();
			});

			return Result;
		}
#endif
#ifdef VI_ALLOCATOR
		template <typename O, typename I>
		inline O Copy(const I& Other)
		{
			static_assert(!std::is_same<I, O>::value, "copy should be used to copy object of the same type with different allocator");
			static_assert(IsIterable<I>::value, "input type should be iterable");
			static_assert(IsIterable<O>::value, "output type should be iterable");
			return O(Other.begin(), Other.end());
		}
#else
		template <typename O, typename I>
		inline O&& Copy(I&& Other)
		{
			static_assert(IsIterable<I>::value, "input type should be iterable");
			static_assert(IsIterable<O>::value, "output type should be iterable");
			return std::move(Other);
		}
		template <typename O, typename I>
		inline const O& Copy(const I& Other)
		{
			static_assert(IsIterable<I>::value, "input type should be iterable");
			static_assert(IsIterable<O>::value, "output type should be iterable");
			return Other;
		}
#endif
#ifdef VI_CXX17
		template <typename T>
		inline Expected<T> FromStringRadix(const String& Other, int Base)
		{
			static_assert(std::is_integral<T>::value, "base can be specified only for integral types");
			T Value;
			std::from_chars_result Result = std::from_chars(Other.data(), Other.data() + Other.size(), Value, Base);
			if (Result.ec != std::errc())
				return std::make_error_condition(Result.ec);
			return Value;
		}
		template <typename T>
		inline Expected<T> FromString(const String& Other)
		{
			static_assert(std::is_integral<T>::value, "conversion can be done only for integral types");
			T Value;
			std::from_chars_result Result = std::from_chars(Other.data(), Other.data() + Other.size(), Value);
			if (Result.ec != std::errc())
				return std::make_error_condition(Result.ec);
			return Value;
		}
#else
		template <typename T>
		inline Expected<T> FromStringRadix(const String& Other, int Base)
		{
			static_assert(false, "conversion can be done only to arithmetic types");
			return std::make_error_condition(std::errc::not_supported);
		}
		template <>
		inline Expected<int8_t> FromStringRadix<int8_t>(const String& Other, int Base)
		{
			OS::Error::Clear();
			char* End = nullptr;
			const char* Start = Other.c_str();
			long Value = strtol(Start, &End, Base);
			if (Start == End)
				return std::make_error_condition(std::errc::invalid_argument);
			else if (Value < (long)std::numeric_limits<int8_t>::min())
				return std::make_error_condition(std::errc::result_out_of_range);
			else if (Value > (long)std::numeric_limits<int8_t>::max())
				return std::make_error_condition(std::errc::result_out_of_range);
			else if (OS::Error::Occurred())
				return OS::Error::GetCondition();
			return (int8_t)Value;
		}
		template <>
		inline Expected<int16_t> FromStringRadix<int16_t>(const String& Other, int Base)
		{
			OS::Error::Clear();
			char* End = nullptr;
			const char* Start = Other.c_str();
			long Value = strtol(Start, &End, Base);
			if (Start == End)
				return std::make_error_condition(std::errc::invalid_argument);
			else if (Value < (long)std::numeric_limits<int16_t>::min())
				return std::make_error_condition(std::errc::result_out_of_range);
			else if (Value > (long)std::numeric_limits<int16_t>::max())
				return std::make_error_condition(std::errc::result_out_of_range);
			else if (OS::Error::Occurred())
				return OS::Error::GetCondition();
			return (int16_t)Value;
		}
		template <>
		inline Expected<int32_t> FromStringRadix<int32_t>(const String& Other, int Base)
		{
			OS::Error::Clear();
			char* End = nullptr;
			const char* Start = Other.c_str();
			long Value = strtol(Start, &End, Base);
			if (Start == End)
				return std::make_error_condition(std::errc::invalid_argument);
			else if (OS::Error::Occurred())
				return OS::Error::GetCondition();
			return (int32_t)Value;
		}
		template <>
		inline Expected<int64_t> FromStringRadix<int64_t>(const String& Other, int Base)
		{
			OS::Error::Clear();
			char* End = nullptr;
			const char* Start = Other.c_str();
			long long Value = strtoll(Start, &End, Base);
			if (Start == End)
				return std::make_error_condition(std::errc::invalid_argument);
			else if (OS::Error::Occurred())
				return OS::Error::GetCondition();
			return (int64_t)Value;
		}
		template <>
		inline Expected<uint8_t> FromStringRadix<uint8_t>(const String& Other, int Base)
		{
			OS::Error::Clear();
			char* End = nullptr;
			const char* Start = Other.c_str();
			unsigned long Value = strtoul(Start, &End, Base);
			if (Start == End)
				return std::make_error_condition(std::errc::invalid_argument);
			else if (Value > (unsigned long)std::numeric_limits<uint8_t>::max())
				return std::make_error_condition(std::errc::result_out_of_range);
			else if (OS::Error::Occurred())
				return OS::Error::GetCondition();
			return (uint8_t)Value;
		}
		template <>
		inline Expected<uint16_t> FromStringRadix<uint16_t>(const String& Other, int Base)
		{
			OS::Error::Clear();
			char* End = nullptr;
			const char* Start = Other.c_str();
			unsigned long Value = strtoul(Start, &End, Base);
			if (Start == End)
				return std::make_error_condition(std::errc::invalid_argument);
			else if (Value > (unsigned long)std::numeric_limits<uint16_t>::max())
				return std::make_error_condition(std::errc::result_out_of_range);
			else if (OS::Error::Occurred())
				return OS::Error::GetCondition();
			return (uint16_t)Value;
		}
		template <>
		inline Expected<uint32_t> FromStringRadix<uint32_t>(const String& Other, int Base)
		{
			OS::Error::Clear();
			char* End = nullptr;
			const char* Start = Other.c_str();
			unsigned long Value = strtoul(Start, &End, Base);
			if (Start == End)
				return std::make_error_condition(std::errc::invalid_argument);
			else if (OS::Error::Occurred())
				return OS::Error::GetCondition();
			return (uint32_t)Value;
		}
		template <>
		inline Expected<uint64_t> FromStringRadix<uint64_t>(const String& Other, int Base)
		{
			OS::Error::Clear();
			char* End = nullptr;
			const char* Start = Other.c_str();
			unsigned long long Value = strtoull(Start, &End, Base);
			if (Start == End)
				return std::make_error_condition(std::errc::invalid_argument);
			else if (OS::Error::Occurred())
				return OS::Error::GetCondition();
			return (uint64_t)Value;
		}
		template <typename T>
		inline Expected<T> FromString(const String& Other)
		{
			static_assert(false, "conversion can be done only to arithmetic types");
			return std::make_error_condition(std::errc::not_supported);
		}
		template <>
		inline Expected<int8_t> FromString<int8_t>(const String& Other)
		{
			return FromStringRadix<int8_t>(Other, 10);
		}
		template <>
		inline Expected<int16_t> FromString<int16_t>(const String& Other)
		{
			return FromStringRadix<int16_t>(Other, 10);
		}
		template <>
		inline Expected<int32_t> FromString<int32_t>(const String& Other)
		{
			return FromStringRadix<int32_t>(Other, 10);
		}
		template <>
		inline Expected<int64_t> FromString<int64_t>(const String& Other)
		{
			return FromStringRadix<int64_t>(Other, 10);
		}
		template <>
		inline Expected<uint8_t> FromString<uint8_t>(const String& Other)
		{
			return FromStringRadix<uint8_t>(Other, 10);
		}
		template <>
		inline Expected<uint16_t> FromString<uint16_t>(const String& Other)
		{
			return FromStringRadix<uint16_t>(Other, 10);
		}
		template <>
		inline Expected<uint32_t> FromString<uint32_t>(const String& Other)
		{
			return FromStringRadix<uint32_t>(Other, 10);
		}
		template <>
		inline Expected<uint64_t> FromString<uint64_t>(const String& Other)
		{
			return FromStringRadix<uint64_t>(Other, 10);
		}
#endif
		template <>
		inline Expected<float> FromString<float>(const String& Other)
		{
			OS::Error::Clear();
			char* End = nullptr;
			const char* Start = Other.c_str();
			float Value = strtof(Start, &End);
			if (Start == End)
				return std::make_error_condition(std::errc::invalid_argument);
			else if (OS::Error::Occurred())
				return OS::Error::GetCondition();
			return Value;
		}
		template <>
		inline Expected<double> FromString<double>(const String& Other)
		{
			OS::Error::Clear();
			char* End = nullptr;
			const char* Start = Other.c_str();
			double Value = strtod(Start, &End);
			if (Start == End)
				return std::make_error_condition(std::errc::invalid_argument);
			else if (OS::Error::Occurred())
				return OS::Error::GetCondition();
			return Value;
		}
		template <>
		inline Expected<long double> FromString<long double>(const String& Other)
		{
			OS::Error::Clear();
			char* End = nullptr;
			const char* Start = Other.c_str();
			long double Value = strtold(Start, &End);
			if (Start == End)
				return std::make_error_condition(std::errc::invalid_argument);
			else if (OS::Error::Occurred())
				return OS::Error::GetCondition();
			return Value;
		}
		template <typename T>
		inline String ToString(T Other)
		{
			static_assert(std::is_arithmetic<T>::value, "conversion can be done only to arithmetic types");
			return Copy<String, std::string>(std::to_string(Other));
		}
	}
}
#endif