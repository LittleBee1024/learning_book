const CustomInput = {
   props: ['modelValue'],
   emits: ['update:modelValue'],
   template:`
      <input
         :value="modelValue"
         @input="$emit('update:modelValue', $event.target.value)"
      />
   `
}

const RootComponent = {
   data() {
      return {
         searchText: ''
      }
   },
   components: {
      "custom-input": CustomInput
   }
}

const vm = Vue.createApp(RootComponent).mount('#app')
