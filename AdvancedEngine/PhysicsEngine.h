#pragma once
#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>

// --- Boilerplate --- //

namespace Layers {
	static constexpr JPH::ObjectLayer NON_MOVING = 0;
	static constexpr JPH::ObjectLayer MOVING = 1;
	static constexpr JPH::ObjectLayer NUM_LAYERS = 2;
}

class ObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter {
public:
	virtual bool ShouldCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2) const override {
		switch (inObject1) {
		case Layers::NON_MOVING:
			return inObject2 == Layers::MOVING;
		case Layers::MOVING:
			return true;
		default:
			JPH_ASSERT(false);
			return false;
		}
	}
};

namespace BroadPhaseLayers {
	static constexpr JPH::BroadPhaseLayer NON_MOVING(0);
	static constexpr JPH::BroadPhaseLayer MOVING(1);
	static constexpr JPH::uint NUM_LAYERS(2);
}


class BPLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface
{
public:
	BPLayerInterfaceImpl()
	{
		// map object layers to broadphase layers
		mObjectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
		mObjectToBroadPhase[Layers::MOVING] = BroadPhaseLayers::MOVING;
	}

	// required overrides
	virtual JPH::uint GetNumBroadPhaseLayers() const override
	{
		return BroadPhaseLayers::NUM_LAYERS;
	}

	virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override
	{
		JPH_ASSERT((JPH::uint)inLayer < Layers::NUM_LAYERS);
		return mObjectToBroadPhase[(JPH::uint)inLayer];
	}

	// **This was missing in your code**
	virtual const char* GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override
	{
		// Return a human readable name for debug. Use inLayer.GetValue() or cast to uint.
		switch (inLayer.GetValue())
		{
		case 0: return "NON_MOVING";
		case 1: return "MOVING";
		default: return "UNKNOWN_BROADPHASE_LAYER";
		}
	}

private:
	JPH::BroadPhaseLayer mObjectToBroadPhase[Layers::NUM_LAYERS];
};

class ObjectVsBroadPhaseLayerFilterImpl : public JPH::ObjectVsBroadPhaseLayerFilter
{
public:
	virtual bool				ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const override
	{
		switch (inLayer1)
		{
		case Layers::NON_MOVING:
			return inLayer2 == BroadPhaseLayers::MOVING;
		case Layers::MOVING:
			return true;
		default:
			JPH_ASSERT(false);
			return false;
		}
	}
};


///  ---- Physics engine ----

class PhysicsEngine {
public:
	PhysicsEngine();
	~PhysicsEngine();

	void initialize();
	void addObject(JPH::Body* body);

	void step(float deltaTime);
	JPH::Vec3 getBodyLocation(JPH::Body& b);
	void bodyWriteVelocity(JPH::Vec3& velocity, JPH::Body& b);
	JPH::Vec3 bodyReadVelocity(JPH::Body& b);
	void bodyWriteAngularVelocity(JPH::Vec3& angularVelocity, JPH::Body& b);


	JPH::BodyInterface* bodyInterface;
private:
	JPH::TempAllocatorImpl* tempAllocator;
	JPH::PhysicsSystem* physicsSystem;
	JPH::JobSystemThreadPool* jobSystem;
	JPH::Factory* sInstance;

	BPLayerInterfaceImpl broadPhaseLayerInterface;
	ObjectVsBroadPhaseLayerFilterImpl objectVsBroadphaseLayerFilter;
	ObjectLayerPairFilterImpl objectVsObjectLayerFilter;
	

};