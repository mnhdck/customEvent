//Event Define
#define Event_Def(eventName, ...) \
public: \
	template<typename... ARGS> \
	class E_##eventName{ \
	public: \
		E_##eventName():m_bIsSending(false){} \
		typedef void (EventAccepter::*EventType)(__VA_ARGS__); \
		EventType eType; \
		struct Info{ \
			EventType fun; \
			EventAccepter* accepter; \
			int offset; \
			Info():fun(NULL), accepter(NULL), offset(0){} \
		}; \
			void Add(EventAccepter* accepter, EventType func, int pointerOffset) \
		{ \
		for (Info info : m_vecList){ if (info.accepter == accepter && info.fun == func) return; } \
			Info info; info.accepter = accepter; info.fun = func; info.offset = pointerOffset; \
			if(m_bIsSending) m_vecAdd.push_back(info); \
			else m_vecList.push_back(info); \
		} \
		void Send(ARGS... args){ \
			m_bIsSending = true; \
			for (Info& info : m_vecList){ ((info.accepter+info.offset)->*info.fun)(args...); } \
			m_bIsSending = false; \
			std::copy(m_vecAdd.begin(), m_vecAdd.end(), std::back_inserter(m_vecList)); m_vecAdd.clear(); \
			for (Info& info : m_vecRemove) { Remove(info.accepter, info.fun);} m_vecRemove.clear(); \
		} \
		void Remove(EventAccepter* accepter, EventType func) \
		{ \
			if(m_bIsSending) { \
				Info info; info.accepter = accepter; info.fun = func; \
				m_vecRemove.push_back(info);} \
			else {for (auto iter = m_vecList.begin(); iter != m_vecList.end(); ){if(iter->accepter ==accepter && iter->fun == func) {iter= m_vecList.erase(iter);}else iter++;}} \
		} \
	private: \
		std::vector<Info> m_vecList; \
		std::vector<Info> m_vecAdd; \
		std::vector<Info> m_vecRemove; \
		bool m_bIsSending; \
	}; \
	E_##eventName<__VA_ARGS__> eventName;

#define ERegister(sender, eventName, accepter, slot) (sender)->eventName.Add(accepter, static_cast<decltype((sender)->eventName.eType)>(slot), PTO_ULONG(accepter) - PTO_ULONG(static_cast<EventAccepter*>(accepter)))
#define EUnRegister(sender, eventName, accepter, slot) (sender)->eventName.Remove(accepter, static_cast<decltype((sender)->eventName.eType)>(slot))
#define ESendEvent(sender, eventName, ...) (sender)->eventName.Send(__VA_ARGS__)

class EventAccepter
{
};
