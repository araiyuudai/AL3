#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include "MathUtilityForText.h"
#include "time.h"


//コンストラクタ
GameScene::GameScene() {}
//デストラクタ
GameScene::~GameScene() {

delete spriteBG_;
	delete modelStage_;

delete modelPlayer_;


delete modelBeam_;

delete modelEnemy_;

}
//初期化
void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

textureHandleBG_ = TextureManager::Load("bg.jpg");
	spriteBG_ = Sprite::Create(textureHandleBG_, {0, 0});

	
	viewProjection_.translation_.y = 1;
	viewProjection_.translation_.z = -6;
	viewProjection_.Initialize();

	textureHandleStage_ = TextureManager::Load("stage.jpg");
	modelStage_ = Model::Create();
	worldTransformStage_.Initialize();

	worldTransformStage_.translation_ = {0, -1.5f, 0};
	worldTransformStage_.scale_ = {4.5f, 1, 40};

	worldTransformStage_.matWorld_ = MakeAffineMatrix(
	    worldTransformStage_.scale_, worldTransformStage_.rotation_,
	    worldTransformStage_.translation_);

	worldTransformStage_.TransferMatrix();


textureHandlePlayer_ = TextureManager::Load("player.png");
modelPlayer_ = Model::Create();

worldTransformPlayer_.scale_ = {0.5f, 0.5f, 0.5f};
worldTransformPlayer_.Initialize();


textureHandleBeam_ = TextureManager::Load("beam.png");
modelBeam_ = Model::Create();

worldTransformBeam_.scale_ = {0.3f, 0.3f, 0.3f};
worldTransformBeam_.Initialize();



textureHandleEnemy_ = TextureManager::Load("enemy.png");
modelEnemy_ = Model::Create();

worldTransformEnemy_.scale_ = {0.5f, 0.5f, 0.5f};
worldTransformEnemy_.Initialize();

srand((unsigned int)time(NULL));

debugText_ = DebugText::GetInstance();
debugText_->Initialize();


}
//更新
void GameScene::Update() { 
	
	PlayerUpdate(); 
	BeamUpdate();
	EnemyUpdate();
	Collision();
}

//描画
void GameScene::Draw() {

	

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>
	spriteBG_->Draw();


	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	modelStage_->Draw(worldTransformStage_, viewProjection_, textureHandleStage_);


	modelPlayer_->Draw(worldTransformPlayer_, viewProjection_, textureHandlePlayer_);

	
	if (beamFlag_ == 1) {
		modelBeam_->Draw(worldTransformBeam_, viewProjection_, textureHandleBeam_);
	}	
	if (enemyFlag_ == 1) {
	modelEnemy_->Draw(worldTransformEnemy_, viewProjection_, textureHandleEnemy_);
	}





	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>


		debugText_->Print("STG", 10, 10, 2);

	debugText_->DrawAll();

	char str[100];

	sprintf_s(str, "SCORE %d", gameScore_);
	debugText_->Print(str, 200, 10, 2);

	sprintf_s(str, "Life %d", playerLife_);
	debugText_->Print(str, 500, 10, 2);



	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

//プレイヤー更新
void GameScene::PlayerUpdate() {
	worldTransformPlayer_.matWorld_ = MakeAffineMatrix(
	    worldTransformPlayer_.scale_, worldTransformPlayer_.rotation_,
	    worldTransformPlayer_.translation_);

	worldTransformPlayer_.TransferMatrix();

	//右へ移動
	if (input_->PushKey(DIK_RIGHT)) {
		worldTransformPlayer_.translation_.x += 0.1f;
	}
	//左に移動
	if (input_->PushKey(DIK_LEFT)) {
		worldTransformPlayer_.translation_.x -= 0.1f;
	}
	if (worldTransformPlayer_.translation_.x > 4) {
		worldTransformPlayer_.translation_.x = 4;
	}
	if (worldTransformPlayer_.translation_.x < -4) {
		worldTransformPlayer_.translation_.x = -4;
	}

}
//ビーム更新
void GameScene::BeamUpdate() {
	worldTransformBeam_.matWorld_ = MakeAffineMatrix(
	    worldTransformBeam_.scale_, worldTransformBeam_.rotation_,
	    worldTransformBeam_.translation_);

	worldTransformBeam_.TransferMatrix();

	BeamMove();
	BeamBorn();

}

void GameScene::BeamMove() {
	
	if (beamFlag_ == 1) {
		worldTransformBeam_.translation_.z += 1.0f;
	}
	if (beamFlag_ == 1) {
		if (worldTransformBeam_.translation_.z > 40) {
			beamFlag_ = 0;

		}
	}
	worldTransformBeam_.rotation_.x += 0.1f;
}

void GameScene::BeamBorn() {

	if (beamFlag_ == 0) {
		if (input_->TriggerKey(DIK_SPACE)) {
			beamFlag_ = 1;
			worldTransformBeam_.translation_.x = worldTransformPlayer_.translation_.x;
			worldTransformBeam_.translation_.z = worldTransformPlayer_.translation_.z;
		
		}
	}
}


void GameScene::EnemyUpdate() {


	worldTransformEnemy_.matWorld_ = MakeAffineMatrix(
	    worldTransformEnemy_.scale_, worldTransformEnemy_.rotation_,
	    worldTransformEnemy_.translation_);

	worldTransformEnemy_.TransferMatrix();
	
	EnemyMove();
	EnemyBorn();
}
void GameScene::EnemyMove() {

	
	worldTransformEnemy_.translation_.z -= 0.5f;
	worldTransformEnemy_.rotation_.x += 0.5f;

	
}

void GameScene::EnemyBorn() {
	    
	if (worldTransformEnemy_.translation_.z < -5) {
	
	
	enemyFlag_ = 0;
	}


	if (enemyFlag_ == 0) {
	
		enemyFlag_ = 1;
	
		
	int x = rand() % 80;

		float x2 = (float)x / 10 - 4;

		worldTransformEnemy_.translation_.x = x2;
	
			
		worldTransformEnemy_.translation_.z = 40; 



	}
	



	
}

void GameScene::Collision() { 
	
	
CollisionPlayerEnemy();

CollisionBeamEnemy();

}

void GameScene::CollisionPlayerEnemy(){

	if (enemyFlag_ == 1) {
	
	float dx = abs(worldTransformPlayer_.translation_.x - worldTransformEnemy_.translation_.x);
	float dz = abs(worldTransformPlayer_.translation_.z - worldTransformEnemy_.translation_.z);

	if (dx < 1 && dz < 1) {
		
		enemyFlag_ = 0;
			playerLife_ -= 1;
	}

	}

}
void GameScene::CollisionBeamEnemy() {

	if (enemyFlag_ == 1) {
	if (beamFlag_ == 1) {
			float dx =
			    abs(worldTransformBeam_.translation_.x - worldTransformEnemy_.translation_.x);
			float dz =
			    abs(worldTransformBeam_.translation_.z - worldTransformEnemy_.translation_.z);



			if (dx < 1 && dz < 1) {
				gameScore_ += 100;
				enemyFlag_ = 0;
				beamFlag_ = 0;	
				
			}
	}
	}
}