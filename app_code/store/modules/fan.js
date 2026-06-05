

const state = {
	// fan1: 0,
	// fan2: 0,
	// fan_spd: 0
	
	spd: 0,
};
const mutations = {
	// 接硬件收风扇状态
	RECEIVESPD(state, value) {
		uni.setStorageSync("spd",JSON.stringify({spd:value.spd}))
		
		state.spd=value.spd
	},
};
const actions = {

};
const getters = {

}
export default {
	namespaced: true,
	state,
	getters,
	actions,
	mutations,
};
