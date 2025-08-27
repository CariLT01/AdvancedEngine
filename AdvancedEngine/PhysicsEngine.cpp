#include "PhysicsEngine.h"

PhysicsEngine::PhysicsEngine() {
	JPH::RegisterDefaultAllocator();
	tempAllocator = new JPH::TempAllocatorImpl(100 * 1024 * 1024);
	sInstance = new JPH::Factory();
	JPH::Factory::sInstance = sInstance;
	JPH::RegisterTypes();
	
	jobSystem = new JPH::JobSystemThreadPool(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, JPH::thread::hardware_concurrency() - 1);
	physicsSystem = new JPH::PhysicsSystem();


	physicsSystem->Init(65536, 0, 65536, 65536, broadPhaseLayerInterface, objectVsBroadphaseLayerFilter, objectVsObjectLayerFilter);
	bodyInterface = (&physicsSystem->GetBodyInterface());
}

void PhysicsEngine::addObject(JPH::Body* body) {
	bodyInterface->AddBody(body->GetID(), JPH::EActivation::Activate);
}

void PhysicsEngine::step(float deltaTime) {
	physicsSystem->Update(deltaTime, 4, tempAllocator, jobSystem);
}

JPH::Vec3 PhysicsEngine::getBodyLocation(JPH::Body& b) {
	JPH::BodyLockRead lock(physicsSystem->GetBodyLockInterface(), b.GetID());
	if (lock.Succeeded()) {
		const JPH::Body & b = lock.GetBody();

		return b.GetCenterOfMassPosition();
	}
	return JPH::Vec3::sZero();
}

void PhysicsEngine::bodyWriteVelocity(JPH::Vec3& v, JPH::Body& b) {
	JPH::BodyLockWrite lock(physicsSystem->GetBodyLockInterface(), b.GetID());
	if (lock.Succeeded()) {
		 JPH::Body& body = lock.GetBody();

		// Make sure it is dynamic
		if (body.GetMotionType() == JPH::EMotionType::Dynamic)
		{
			body.SetLinearVelocity(v);
			//bodyInterface->SetLinearVelocity(body.GetID(), v);

			//body.SetLinearVelocity(v);
			// Optional: set angular velocity too
			// body.SetAngularVelocity(Vec3(0, 0, 0));
		}
	}
}

JPH::Vec3 PhysicsEngine::bodyReadVelocity(JPH::Body& b) {
	JPH::BodyLockRead lock(physicsSystem->GetBodyLockInterface(), b.GetID());
	if (lock.Succeeded()) {
		const JPH::Body& body = lock.GetBody();

		return body.GetLinearVelocity();
	}

	return JPH::Vec3::sZero();
}

void PhysicsEngine::bodyWriteAngularVelocity(JPH::Vec3& velocity, JPH::Body& b) {
	JPH::BodyLockWrite lock(physicsSystem->GetBodyLockInterface(), b.GetID());
	if (lock.Succeeded()) {
		JPH::Body& body = lock.GetBody();

		body.SetAngularVelocity(velocity);
	}
}