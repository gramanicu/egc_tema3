#pragma once

#include <include/glm.h>

/// <summary>
/// This namespace includes all physics related classes, methods etc..
/// The "physics engine" is a primitive 2D engine, and uses semi-implicit
/// Euler or RK4 (Runge-Kutta order 4) integrators.
/// </summary>
namespace GameEngine
{
	typedef struct _State {
		glm::vec3 x;
		glm::vec3 v;

		double drag_coef = 0.1f;
		double gravity_coef = 0.f;
	} State;

	typedef struct _Derivative {
		glm::vec3 dx;
		glm::vec3 dv;
	} Derivative;

	namespace PhysicsConstants {
		const static double G_CONSTANT = 10.f;

		/// <summary>
		/// Defines motion computation types for a RigidBody
		/// </summary>
		enum class Motion_Type {
			/// <summary>
			/// The motion is simulated using forces
			/// </summary>
			SIMULATED,

			/// <summary>
			/// The motion folows a time variant function
			/// </summary>
			FUNCTION
		};
	}

	class RigidBody {
	private:
		/// <summary>
		/// Checks velocity limits and corrects the velocity.
		/// </summary>
		void checkVelLimits();
	public:
		State state;

		/// <summary>
		/// If the physics computations should be enabled for this object.
		/// </summary>
		bool physics_enabled;

		/// <summary>
		/// The motion type for the rigidbody
		/// </summary>
		PhysicsConstants::Motion_Type m_type;
		
		/// <summary>
		/// The movement function for the rigidbody;
		/// </summary>
		void(*m_func)(GameEngine::State& state, double time , double dt);

		/// <summary>
		/// A numerical limit that the magnitude of the velocity vector should not pass.
		/// </summary>
		double velocity_limit;

		RigidBody();

		/// <summary>
		/// Adds an impulse to the rigidbody.
		/// </summary>
		/// <param name="p">The impulse to use</param>
		void addImpulse(glm::vec3 p);

		/// <summary>
		/// Adds an impulse to the rigidbody.
		/// </summary>
		/// <param name="x">The X component of the impulse.</param>
		/// <param name="y">The Y component of the impulse.</param>
		/// <param name="z">The Z component of the impulse.</param>
		void addImpulse(const double x,const double y, const double z);
	};

	class PhysixEngine
	{
	private:
		static double current_time;

		static glm::vec3 compute_acceleration(const State& state, double t);
	public:
		/// <summary>
		/// Evaluates and computes the values for the State and Derivative of the State
		/// in the next iteration
		/// 
		/// For example, to compute the position, we can use:
		///	v(t+1) = v(t) + dv/dt (equivalent to vf = v0 + at).
		/// For this case, v(t) is contained in the initial state and
		/// dv is contained in the d derivative
		/// </summary>
		/// <param name="initial">The initial state. </param>
		/// <param name="t">The total simulation time</param>
		/// <param name="dt">The "deltaTime"</param>
		/// <param name="d">The derivative of the initial state</param>
		/// <returns></returns>
		static Derivative evaluate(const State& initial, double t, float dt, const Derivative& d);
	
		/// <summary>
		/// This function will compute the value of the next state
		/// </summary>
		/// <param name="body">The body that will have the state updated</param>
		/// <param name="t">The total simulation time</param>
		/// <param name="dt">The "deltaTime"</param>
		/// <param name="RK4">If RK4 integration should be used</param>
		static void integrate(RigidBody& body, double t, float dt, bool RK4 = true);

		/// <summary>
		/// Update the physics for the selected state
		/// </summary>
		/// <param name="state">The state to update</param>
		/// <param name="deltaTime"></param>
		static void UpdatePhysics(RigidBody& rigidBody, const double deltaTime);

		/// <summary>
		/// Update the total physics time. Must be called only once per frame
		/// </summary>
		static void PhysixEngine::UpdateTime(const double deltaTime);
	};
} // namespace Physics