#pragma once
#include "TNAH/Core/Core.h"
#include "TNAH/Events/Event.h"
#include "TNAH/Core/Timestep.h"

namespace tnah {

	/**
	 * @class	Layer
	 *
	 * @brief	The layer class is used to allow for the creation of layers, which allow for things to be rendered and/or events to be received on multiple different layers. 
	 *
	 * @author	Dylan Blereau
	 * @date	10/09/2021
	 */

	class TNAH_API Layer
	{
	public:

		/**
		 * @fn	Layer::Layer(const std::string& name = "Layer");
		 *
		 * @brief	Constructor
		 *
		 * @author	Dylan Blereau
		 * @date	10/09/2021
		 *
		 * @param 	name	(Optional) The name.
		 */

		Layer(const std::string& name = "Layer");

		/**
		 * @fn	virtual Layer::~Layer() = default;
		 *
		 * @brief	Defaulted destructor
		 *
		 * @author	Dylan Blereau
		 * @date	10/09/2021
		 */

		virtual ~Layer() = default;

		/**
		 * @fn	virtual void Layer::OnAttach()
		 *
		 * @brief	Executes when the layer is attached
		 *
		 * @author	Dylan Blereau
		 * @date	10/09/2021
		 */

		virtual void OnAttach() {}

		/**
		 * @fn	virtual void Layer::OnDetach()
		 *
		 * @brief	Executes when the layer is detached
		 *
		 * @author	Dylan Blereau
		 * @date	10/09/2021
		 */

		virtual void OnDetach() {}

		/**
		 * @fn	virtual void Layer::OnUpdate(Timestep ts)
		 *
		 * @brief	Executes on update
		 *
		 * @author	Dylan Blereau
		 * @date	10/09/2021
		 *
		 * @param 	ts	The timestep.
		 */

		virtual void OnUpdate(Timestep ts) {}


		/**
		 * @fn	virtual void Layer::OnImGuiRender()
		 *
		 * @brief	Executes when imgui renders
		 *
		 * @author	Dylan Blereau
		 * @date	10/09/2021
		 */

		virtual void OnImGuiRender() {}

		/**
		 * @fn	virtual void Layer::OnEvent(Event& event)
		 *
		 * @brief	Executes when an event occurs
		 *
		 * @author	Dylan Blereau
		 * @date	10/09/2021
		 *
		 * @param [in,out]	event	The event.
		 */

		virtual void OnEvent(Event& event) {}

		/**
		 * @fn	const std::string& Layer::GetName() const
		 *
		 * @brief	Gets the name
		 *
		 * @author	Dylan Blereau
		 * @date	10/09/2021
		 *
		 * @returns	The name.
		 */

		const std::string& GetName() const { return m_DebugName; }
	protected:

		/** @brief	DebugName of the layer */
		std::string m_DebugName;
	};
}

