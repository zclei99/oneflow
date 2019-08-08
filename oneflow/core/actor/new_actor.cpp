#include "oneflow/core/actor/new_actor.h"

namespace oneflow {

namespace actor {

namespace {

void UpdateCtxWithMsg(OpActorCtx* ctx, const ActorMsg& msg) {
  if (msg.msg_type() == ActorMsgType::kRegstMsg) {
    ctx->UpdateWithRegstMsg(msg);
  } else if (msg.msg_type() == ActorMsgType::kEordMsg) {
    ctx->UpdateWithEordMsg(msg);
  } else if (msg.msg_type() == ActorMsgType::kCmdMsg) {
    ctx->UpdateWithCmdMsg(msg);
  } else {
    LOG(FATAL) << "ActorMsgType error";
  }
}

void ActUntilFail(OpActorCtx* ctx) {
  while (ctx->IsReady()) {
    ctx->Act();
    ctx->HandleRegstMsgAfterAct();
  }
}

}  // namespace

int OpActor::HandlerNormal(OpActorCtx* ctx, const ActorMsg& msg) {
  UpdateCtxWithMsg(ctx, msg);
  ActUntilFail(ctx);
  if (ctx->EndOfRead()) { OF_SET_MSG_HANDLER(HandlerZombie); }
}

int OpActor::HandlerZombie(OpActorCtx* ctx, const ActorMsg& msg) {
  ctx->UpdateWithProducedRegstMsg();
  if (ctx->RecvAllProducedMsg()) { OF_SET_MSG_HANDLER(MsgHandler()); }
}

}  // namespace actor

}  // namespace oneflow
