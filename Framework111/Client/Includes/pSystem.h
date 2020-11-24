#pragma once

#include "d3dUtility.h"
#include "Camera.h"

namespace psys
{
	struct Particle
	{
		_vector _position;
		D3DCOLOR _color;
		static const DWORD FVF;
	};

	struct Attribute
	{
		_vector _position;
		_vector _velocity;
		_vector _acceleration;
		float _lifetime;
		float _age;
		D3DXCOLOR _color;
		D3DXCOLOR _colorFade;
		bool _isAlive;
		Attribute()
		{
			_lifetime = 0.0f;
			_age = 0.f;
			_isAlive = true; 
		}
	};
	
	class PSystem
	{
	public:
		PSystem();
		virtual ~PSystem();

		virtual bool init(IDirect3DDevice9* device,
			wchar_t* texFileName);

		virtual void resetParticle(Attribute* attribute);
		virtual void reset();
		virtual void addParticle();

		virtual void update(float timeDelta);

		
		
		virtual void preRender();
		virtual void render();
		virtual void postRender();

		bool isEmpty();
		bool isDead();
	protected:
		virtual void removeDeadParticles();
	protected:
		IDirect3DDevice9* _device;
		_vector _origin;
		d3d::BoundingBox _boundingBox;
		float _emitRate;
		float _size;
		IDirect3DTexture9* _tex;
		IDirect3DVertexBuffer9* _vb;
		std::list<Attribute> _particles;
		int _maxParticles;

		// ���ؽ� ���۰� ������ �� �� �ִ� ��ƼŬ�� ��
		DWORD _vbSize;
		// ���ؽ� ���ۿ��� ���縦 ������ ��ƼŬ ��
		// ���� �ܰ���� ������ (����Ʈ�� �ƴ� ��ƼŬ ����)
		// �ܰ� 1�� 0���� 499 ��� �ܰ�2���� �������� 500�� �ȴ�.
		DWORD _vbOffset;
		// �ϳ��� �ܰ迡 ���ǵ� ��ƼŬ�� ��
		DWORD _vbBatchSize;
	};

	class Snow : public PSystem
	{
	public :
		Snow(d3d::BoundingBox* boundingBox, int numParticles);
		void resetParticle(Attribute* attribute);
		void update(float timeDelta);
	};

	class FireWork : public PSystem
	{
	public :
		FireWork(_vector* origin, int numParticles);
		void resetParticle(Attribute* arrtibute);
		void update(float timeDelta);
		void preRender();
		void postRender();
	};

	class ParticleGun : public PSystem
	{
	public:
		ParticleGun(Camera* camera);
		void resetParticle(Attribute* attribute);
		virtual void update(float timeDelta)override;
	private:
		Camera* _camera;
	};
}


