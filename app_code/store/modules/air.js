
// not use

const state={
	air:null,
	
};
const mutations={
	// 接收硬件空气数据
	RECEIVEAIR(state,value){
		if(value.time<100000){
		this.dispatch("time/CURTIME")
		return
		}
		uni.setStorageSync('air',value.air)
		state.air=value.air
	},
	BEGINAIR(state,value){
		state.air=value
	}
};
const actions={
	
};
const getters={
	
}
export default {
  namespaced: true,
  state,
  getters,
  actions,
  mutations,
};