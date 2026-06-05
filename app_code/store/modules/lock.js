
// not use

import request from '@/utils/request.js'
import {
		lock
	} from '../../utils/mqttControl.js'
const state = {
	isLock: false,
};
const mutations = {
	CHANGELOCK(state,value) {
		state.isLock = value
	},
	
};
const actions = {
	async changeLock({commit},value){
			await request({url:`bicycle/changeState/${value}/2`,method:'post'}).then(({data})=>{
					if(data.code===200)commit('CHANGELOCK',data.curState)
				})
		}
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
