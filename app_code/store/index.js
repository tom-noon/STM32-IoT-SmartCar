import Vue from 'vue'
import Vuex from 'vuex'
import temp from "./modules/temp"
import humi from "./modules/humi"
import mode from './modules/mode'
import fan from './modules/fan'

Vue.use(Vuex)

export default new Vuex.Store({
	strict:true,
	state:{
	},
	mutations:{
	},
	actions:{
	},
	modules:{
		temp,
		humi,
		mode,
		fan,
	}
})

